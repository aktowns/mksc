#include "mks_node.h"
#include <stdio.h>
#include <mks_node.h>
#include <mks_type.h>
#include <stdlib.h>
#include "utils/log_utils.h"
#include "mks_enrichment.h"

static inline void _assert_type(mks_node_t* src, mks_type_resolution_t typ, const char* file, int line) {
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

#define assert_type(src, typ) _assert_type(src, typ, __FILE__, __LINE__)

void enrich_number_op(mks_operator_t* op) {
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
        op->right->type->resolved_type = TY_NUMBER;
    if (op->left->type->state == UNRESOLVED && op->left->tag == NODE_IDENTIFIER)
        op->left->type->resolved_type = TY_NUMBER;

}


void enrich_tree(mks_node_t *tree) {
    if (tree->type->state == RESOLVED) {
        printf("Cowardly refusing to resolve previously resolved type\n");
        return;
    }

    switch (tree->tag) {
        case NODE_IDENTIFIER:
            break;
        case NODE_NUMBER_LITERAL:
            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;

            break;
        case NODE_STRING_LITERAL:
            tree->type->resolved_type = TY_STRING;
            tree->type->state = RESOLVED;

            break;
        case NODE_ARRAY_LITERAL: {
            enrich_tree(tree->array->items);

            mks_copy_type(tree->array->items->type, tree->type->value->single);

            // TODO: type check all elements are the same.

            tree->type->resolved_type = TY_ARRAY;
            tree->type->state = RESOLVED;
            tree->type->kind = UNARY;
            break;
        }
        case NODE_MODULE:
            enrich_tree(tree->module->body);

            tree->type->resolved_type = TY_MODULE;
            tree->type->state = RESOLVED;

            tree->module->name->type->resolved_type = TY_MODULE;
            tree->module->name->type->state = RESOLVED;

            tree->module->body->type->resolved_type = TY_UNIT;
            tree->module->body->type->state = RESOLVED;

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

            tree->type->resolved_type = TY_TUPLE;
            tree->type->state = RESOLVED;
            break;
        case NODE_ASSIGNMENT:
            enrich_tree(tree->assignment->value);
            enrich_tree(tree->assignment->identifier);

            mks_copy_type(tree->assignment->value->type, tree->assignment->identifier->type);
            mks_copy_type(tree->assignment->value->type, tree->type);

            break;
        case NODE_IMPORT:
            enrich_tree(tree->import->name);
            enrich_tree(tree->import->qualified);
            enrich_tree(tree->import->specific);

            tree->import->name->type->resolved_type = TY_MODULE;
            tree->import->name->type->state = RESOLVED;

            tree->type->resolved_type = TY_MODULE;
            tree->type->state = RESOLVED;
            break;
        case NODE_FUNCTION_CALL:
            enrich_tree(tree->function_call->name);
            enrich_tree(tree->function_call->arguments);

            break;
        case NODE_FUNCTION:
            enrich_tree(tree->function->argument);
            enrich_tree(tree->function->body);

            tree->type->resolved_type = TY_FUNCTION;
            tree->type->state = RESOLVED;
            tree->type->kind = BINARY;

            mks_copy_type(tree->function->argument->type, tree->type->value->tuple->one);
            mks_copy_type(tree->function->body->type, tree->type->value->tuple->two);

            break;
        case NODE_IF:
            enrich_tree(tree->if_stmt->condition);
            enrich_tree(tree->if_stmt->true_body);
            enrich_tree(tree->if_stmt->false_body);

            assert_type(tree->if_stmt->condition, TY_BOOL);
            // both branches must return the same type
            assert_type(tree->if_stmt->false_body, tree->if_stmt->true_body->type->resolved_type);

            mks_copy_type(tree->if_stmt->true_body->type, tree->type);

            break;
        case NODE_EQ_OP:
            enrich_tree(tree->eq_op->left);
            enrich_tree(tree->eq_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_NE_OP:
            enrich_tree(tree->ne_op->left);
            enrich_tree(tree->ne_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_LT_OP:
            enrich_number_op(tree->lt_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->lt_op->left, TY_NUMBER);
            assert_type(tree->lt_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_GT_OP:
            enrich_number_op(tree->gt_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->gt_op->left, TY_NUMBER);
            assert_type(tree->gt_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_LE_OP:
            enrich_number_op(tree->le_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->le_op->left, TY_NUMBER);
            assert_type(tree->le_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_GE_OP:
            enrich_number_op(tree->ge_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->ge_op->left, TY_NUMBER);
            assert_type(tree->ge_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_PLUS_OP:
            enrich_number_op(tree->plus_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->plus_op->left, TY_NUMBER);
            assert_type(tree->plus_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;

            break;
        case NODE_MINUS_OP:
            enrich_number_op(tree->minus_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->minus_op->left, TY_NUMBER);
            assert_type(tree->minus_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_MULT_OP:
            enrich_number_op(tree->mult_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->minus_op->left, TY_NUMBER);
            assert_type(tree->minus_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_DIVIDE_OP:
            enrich_number_op(tree->divide_op);

            // if we hit this, identifiers have realised to another value or you're using non-number literals.
            assert_type(tree->divide_op->left, TY_NUMBER);
            assert_type(tree->divide_op->right, TY_NUMBER);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_EMPTY:
            tree->type->resolved_type = TY_UNIT;
            tree->type->state = RESOLVED;
            break;
    }
}