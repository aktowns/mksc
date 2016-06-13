// mks_enrichment.c
// Copyright (C) 2016 Ashley Towns
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <mks_node.h>

#include "mks_enrichment.h"

static inline void _assert_type(mks_node_t *src, mks_type_resolution_t typ, const char *file, int line) {
    if (src->type->resolved_type != typ) {
        char *type = prettify_type_resolution(typ);
        char *node = pretty_stringify_value(src);
        char *node_type = pretty_stringify_type(src->type);

        printf("TypeError: Expected %s but %s is of type %s (%s:%i)\n", type, node, node_type, file, line);

        free(node);
        free(node_type);
        free(type);
        exit(-1);
    }
}

static inline void _assert_resolved(mks_node_t *src, const char *file, int line) {
    // char *node = pretty_stringify_value(src);
    char *node = pretty_stringify_type(src->type);

    if (src->type->state != RESOLVED && src->type->kind == NULLARY) {
        printf("TypeError: %s is undefined (%s:%i)\n", node, file, line);

        free(node);
        exit(-1);
    } else if (src->type->kind == UNARY && src->type->value->single->state != RESOLVED) {
        // TODO: Pull more info
        printf("TypeError: %s is undefined (%s:%i)\n", node, file, line);
        free(node);
        exit(-1);
    } else if (src->type->kind == BINARY && (src->type->value->tuple->one->state != RESOLVED ||
                                             src->type->value->tuple->two->state != RESOLVED)) {
        // TODO: Likewise to the above
        printf("TypeError: %s is undefined (%s:%i)\n", node, file, line);
        free(node);
        exit(-1);
    }

    free(node);
}

#define assert_type(src, typ) _assert_type(src, typ, __FILE__, __LINE__)
#define assert_resolved(src) _assert_resolved(src, __FILE__, __LINE__)

void set_type(mks_node_t **node, mks_type_resolution_t res, mks_type_state_t state) {
    if ((*node)->type->resolved_type == res && (*node)->type->state == state) {
        return;
    }

    if ((*node)->type->state == RESOLVED) {
        printf("Cowardly refusing to override type for:\n");
        pretty_print_node(*node);
        printf("\n\n");
        return;
    }

    (*node)->type->resolved_type = res;
    (*node)->type->state = state;
}

void enrich_number_op(mks_operator_t *op) {
    enrich_tree(op->left);
    enrich_tree(op->right);

    // if left is unresolved assume the value of right and vice versa.
    if (op->right->type->state == RESOLVED && op->left->type->state == UNRESOLVED) {
        mks_copy_type(op->right->type, op->left->type);
    } else if (op->right->type->state == UNRESOLVED && op->left->type->state == RESOLVED) {
        mks_copy_type(op->left->type, op->right->type);
    }

    // at this point we may not know what left OR right are, since we're using + which is hardcoded at the moment
    // we're going to force realisation of number on both sides for identifiers, otherwise type error.
    if (op->right->type->state == UNRESOLVED && op->right->tag == NODE_IDENTIFIER)
        set_type(&(op->right), TY_NUMBER, RESOLVED);
    if (op->left->type->state == UNRESOLVED && op->left->tag == NODE_IDENTIFIER)
        set_type(&(op->left), TY_NUMBER, RESOLVED);
}

mks_node_t *try_get_node_from_space(char *identifier, mks_node_t *node) {
    mks_space_t *space = NULL;
    HASH_FIND_STR(node->space, identifier, space);
    if (space != NULL) {
        return space->ref;
    }

    return NULL;
}

void enrich_tree(mks_node_t *tree) {
    if (tree->type->state == RESOLVED) {
        return;
    }

    switch (tree->tag) {
        case NODE_IDENTIFIER:
            break;
        case NODE_NUMBER_LITERAL:
            set_type(&tree, TY_NUMBER, RESOLVED);
            break;
        case NODE_STRING_LITERAL:
            set_type(&tree, TY_STRING, RESOLVED);
            break;
        case NODE_BOOL:
            set_type(&tree, TY_BOOL, RESOLVED);
            break;
        case NODE_ARRAY_LITERAL: {
            enrich_tree(tree->array->items);

            mks_copy_type(tree->array->items->type, tree->type->value->single);

            // TODO: type check all elements are the same.

            set_type(&tree, TY_ARRAY, RESOLVED);
            tree->type->kind = UNARY;
            break;
        }
        case NODE_MODULE:
            enrich_tree(tree->module->body);

            set_type(&tree, TY_MODULE, RESOLVED);
            set_type(&(tree->module->name), TY_MODULE, RESOLVED);

            break;
        case NODE_SEQUENCE:
            enrich_tree(tree->sequence->left);
            enrich_tree(tree->sequence->right);

            // last field in sequence is the return type
            mks_copy_type(tree->sequence->right->type, tree->type);
            break;
        case NODE_TUPLE:
            enrich_tree(tree->tuple->one);
            enrich_tree(tree->tuple->two);

            mks_copy_type(tree->tuple->one->type, tree->type->value->tuple->one);
            mks_copy_type(tree->tuple->two->type, tree->type->value->tuple->two);

            set_type(&tree, TY_TUPLE, RESOLVED);
            break;
        case NODE_ASSIGNMENT:
            enrich_tree(tree->assignment->value);

            mks_copy_type(tree->assignment->value->type, tree->assignment->name->type);
            mks_copy_type(tree->assignment->value->type, tree->type);
            break;
        case NODE_IMPORT:
            enrich_tree(tree->import->name);
            enrich_tree(tree->import->qualified);
            enrich_tree(tree->import->specific);

            set_type(&(tree->import->name), TY_MODULE, RESOLVED);
            set_type(&tree, TY_MODULE, RESOLVED);
            break;
        case NODE_FUNCTION_CALL:
            enrich_tree(tree->function_call->name);
            enrich_tree(tree->function_call->arguments);
            break;
        case NODE_FUNCTION:
            enrich_tree(tree->function->argument);
            enrich_tree(tree->function->body);

            if (tree->function->argument->type->state == UNRESOLVED) {
                mks_node_t *node = try_get_node_from_space(tree->function->argument->identifier, tree);
                if (node != NULL) {
                    printf("was able to find a resolver for %s in space: ", tree->function->argument->identifier);
                    enrich_tree(node);
                    if (node->type->state == UNRESOLVED && node->tag == NODE_IDENTIFIER) {
                        printf("both nodes are unsure of our types, this is fatal.\n");
                    } else if (node->type->state == RESOLVED) {
                        printf("have been told my type, continuing.\n");
                        mks_copy_type(node->type, tree->function->argument->type);
                    }
                }
            }

            set_type(&tree, TY_FUNCTION, RESOLVED);
            tree->type->kind = BINARY;

            mks_copy_type(tree->function->argument->type, tree->type->value->tuple->one);
            mks_copy_type(tree->function->body->type, tree->type->value->tuple->two);

            assert_resolved(tree);
            break;
        case NODE_IF:
            enrich_tree(tree->if_stmt->condition);
            enrich_tree(tree->if_stmt->true_body);
            enrich_tree(tree->if_stmt->false_body);

            if (tree->if_stmt->condition->tag == NODE_IDENTIFIER &&
                tree->if_stmt->condition->type->state == UNRESOLVED) {
                mks_node_t *node = try_get_node_from_space(tree->if_stmt->condition->identifier, tree);
                if (node != NULL) {
                    printf("was able to find resolver for %s in space: ", tree->if_stmt->condition->identifier);
                    enrich_tree(node);
                    if (node->type->state == UNRESOLVED && node->tag == NODE_IDENTIFIER) {
                        printf("both nodes are unsure of our types, inferring bool from if expression context\n");
                        set_type(&tree->if_stmt->condition, TY_BOOL, RESOLVED);
                        set_type(&node, TY_BOOL, RESOLVED);
                    } else if (node->type->state == RESOLVED) {
                        printf("have been told my type, continuing.\n");
                        mks_copy_type(node->type, tree->if_stmt->condition->type);
                    }
                }
            }

            assert_resolved(tree->if_stmt->condition);
            assert_type(tree->if_stmt->condition, TY_BOOL);
            // both branches must return the same type
            assert_type(tree->if_stmt->false_body, tree->if_stmt->true_body->type->resolved_type);

            mks_copy_type(tree->if_stmt->true_body->type, tree->type);
            break;
        case NODE_OPERATOR: {
            switch (tree->operator->op_type) {
                case OP_EQ:
                case OP_NE:
                    enrich_tree(tree->operator->left);
                    enrich_tree(tree->operator->right);

                    set_type(&tree, TY_BOOL, RESOLVED);
                    break;
                case OP_LT:
                case OP_GT:
                case OP_LE:
                case OP_GE:
                    enrich_number_op(tree->operator);

                    // if we hit this, identifiers have realised to another value or you're using non-number literals.
                    assert_type(tree->operator->left, TY_NUMBER);
                    assert_type(tree->operator->right, TY_NUMBER);

                    set_type(&tree, TY_BOOL, RESOLVED);
                    break;
                case OP_PLUS:
                case OP_MINUS:
                case OP_MULT:
                case OP_DIVIDE:
                    enrich_number_op(tree->operator);

                    // if we hit this, identifiers have realised to another value or you're using non-number literals.
                    assert_type(tree->operator->left, TY_NUMBER);
                    assert_type(tree->operator->right, TY_NUMBER);

                    set_type(&tree, TY_NUMBER, RESOLVED);
                    break;
            }
            break;
        }
        case NODE_EMPTY:
            set_type(&tree, TY_UNIT, RESOLVED);
            break;
    }
}
