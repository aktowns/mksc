#include "mks_node.h"
#include <stdio.h>
#include <mks_node.h>
#include <mks_type.h>

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
            enrich_tree(tree->lt_op->left);
            enrich_tree(tree->lt_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_GT_OP:
            enrich_tree(tree->gt_op->left);
            enrich_tree(tree->gt_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_LE_OP:
            enrich_tree(tree->le_op->left);
            enrich_tree(tree->le_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_GE_OP:
            enrich_tree(tree->ge_op->left);
            enrich_tree(tree->ge_op->right);

            tree->type->resolved_type = TY_BOOL;
            tree->type->state = RESOLVED;
            break;
        case NODE_PLUS_OP:
            enrich_tree(tree->plus_op->left);
            enrich_tree(tree->plus_op->right);

            if (tree->plus_op->right->type->state == RESOLVED &&
                    tree->plus_op->left->type->state == UNRESOLVED) {
                mks_copy_type(tree->plus_op->right->type, tree->plus_op->left->type);
            }

            if (tree->plus_op->right->type->state == UNRESOLVED &&
                tree->plus_op->left->type->state == RESOLVED) {
                mks_copy_type(tree->plus_op->left->type, tree->plus_op->right->type);
            }
            
            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;

            break;
        case NODE_MINUS_OP:
            enrich_tree(tree->minus_op->left);
            enrich_tree(tree->minus_op->right);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_MULT_OP:
            enrich_tree(tree->mult_op->left);
            enrich_tree(tree->mult_op->right);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_DIVIDE_OP:
            enrich_tree(tree->divide_op->left);
            enrich_tree(tree->divide_op->right);

            tree->type->resolved_type = TY_NUMBER;
            tree->type->state = RESOLVED;
            break;
        case NODE_EMPTY:
            tree->type->resolved_type = TY_UNIT;
            tree->type->state = RESOLVED;
            break;
    }
}