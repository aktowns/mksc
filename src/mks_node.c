#include "mks_node.h"
#include "utils/log_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mks_node.h>

mks_node_t *mk_node(mks_node_type tag) {
    mks_node_t *node = malloc(sizeof(mks_node_t));

    node->tag = tag;
    node->is_ok = true;

    return node;
}

mks_node_t *mk_identifier(char *identifier) {
    ASSERT(identifier != NULL, "handed a null identifier!");

    mks_node_t *node = mk_node(IDENTIFIER);
    mks_identifier_t *id_node = malloc(sizeof(mks_identifier_t));

    id_node->value = strdup(identifier);
    node->identifier = id_node;

    return node;
}

mks_node_t *mk_module(mks_node_t *identifier,  mks_node_t *body) {
    mks_node_t *node = mk_node(MODULE);
    mks_module_t *mod_node = malloc(sizeof(mks_module_t));

    mod_node->name = identifier;
    mod_node->body = body;
    node->module = mod_node;

    return node;
}

mks_node_t *mk_import(mks_node_t *name, mks_node_t *qualified, mks_node_t *specific) {
    mks_node_t *node = mk_node(IMPORT);
    mks_import_t *imp_node = malloc(sizeof(mks_import_t));

    imp_node->name = name;
    imp_node->qualified = qualified;
    imp_node->specific = specific;
    node->import = imp_node;

    return node;
}

mks_node_t *mk_function_call(mks_node_t *name, mks_node_t *args) {
    mks_node_t *node = mk_node(FUNCTION_CALL);
    mks_function_call_t *func_call = malloc(sizeof(mks_function_call_t));

    func_call->name = name;
    func_call->arguments = args;
    node->function_call = func_call;

    return node;
}

mks_node_t *mk_function(mks_node_t *args, mks_node_t *body) {
    mks_node_t *node = mk_node(FUNCTION);
    mks_function_t *func = malloc(sizeof(mks_function_t));

    func->arguments = args;
    func->body = body;
    node->function = func;

    return node;
}

mks_node_t *mk_string(char *identifier) {
    ASSERT(identifier != NULL, "handed a null identifier!");

    mks_node_t *node = mk_node(STRING_LITERAL);
    mks_string_t *str_node = malloc(sizeof(mks_identifier_t));

    str_node->value = strdup(identifier);
    node->string = str_node;

    return node;
}

mks_node_t *mk_number(int number) {
    mks_node_t *node = mk_node(NUMBER_LITERAL);

    mks_number_t *num_node = malloc(sizeof(mks_number_t));
    num_node->value = number;
    node->number = num_node;

    return node;
}

// WARN: this is a shallow copy
mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(SEQUENCE);

    mks_sequence_t *seq = malloc(sizeof(mks_sequence_t));
    seq->left = left;
    seq->right = right;
    node->sequence = seq;

    return node;
}

mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value) {
    mks_node_t *node = mk_node(ASSIGNMENT);

    mks_assignment_t *ass = malloc(sizeof(mks_assignment_t));
    ass->identifier = name;
    ass->value = value;
    node->assignment = ass;

    return node;
}

mks_node_t *mk_if_expr(mks_node_t *condition, mks_node_t *true_body, mks_node_t *false_body) {
    mks_node_t *node = mk_node(IF_STMT);

    mks_if_stmt_t *ifst = malloc(sizeof(mks_if_stmt_t));
    ifst->condition = condition;
    ifst->true_body = true_body;
    ifst->false_body = false_body;
    node->if_stmt = ifst;

    return node;
}

mks_node_t *mk_eq_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(EQ_OP);

    mks_eq_operator_t *eq = malloc(sizeof(mks_eq_operator_t));
    eq->left = left;
    eq->right = right;
    node->eq_op = eq;

    return node;
}

mks_node_t *mk_ne_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(NE_OP);

    mks_ne_operator_t *ne = malloc(sizeof(mks_ne_operator_t));
    ne->left = left;
    ne->right = right;
    node->ne_op = ne;

    return node;
}

mks_node_t *mk_gt_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(GT_OP);

    mks_gt_operator_t *gt = malloc(sizeof(mks_gt_operator_t));
    gt->left = left;
    gt->right = right;
    node->gt_op = gt;

    return node;
}

mks_node_t *mk_lt_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(LT_OP);

    mks_lt_operator_t *lt = malloc(sizeof(mks_lt_operator_t));
    lt->left = left;
    lt->right = right;
    node->lt_op = lt;

    return node;
}

mks_node_t *mk_ge_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(GE_OP);

    mks_ge_operator_t *ge = malloc(sizeof(mks_ge_operator_t));
    ge->left = left;
    ge->right = right;
    node->ge_op = ge;

    return node;
}

mks_node_t *mk_le_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(LE_OP);

    mks_le_operator_t *le = malloc(sizeof(mks_le_operator_t));
    le->left = left;
    le->right = right;
    node->le_op = le;

    return node;
}

mks_node_t *mk_plus_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(PLUS_OP);

    mks_plus_operator_t *plus = malloc(sizeof(mks_plus_operator_t));
    plus->left = left;
    plus->right = right;
    node->plus_op = plus;

    return node;
}

mks_node_t *mk_minus_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(MINUS_OP);

    mks_minus_operator_t *minus = malloc(sizeof(mks_minus_operator_t));
    minus->left = left;
    minus->right = right;
    node->minus_op = minus;

    return node;
}

mks_node_t *mk_mult_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(MULT_OP);

    mks_mult_operator_t *mult = malloc(sizeof(mks_mult_operator_t));
    mult->left = left;
    mult->right = right;
    node->mult_op = mult;

    return node;
}

mks_node_t *mk_divide_operator(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(DIVIDE_OP);

    mks_divide_operator_t *divide = malloc(sizeof(mks_divide_operator_t));
    divide->left = left;
    divide->right = right;
    node->divide_op = divide;

    return node;
}

void mks_free(mks_node_t *node) {
    if (node == NULL)
        return;

    switch (node->tag) {
        case MODULE:
            mks_free(node->module->name);
            mks_free(node->module->body);
            free(node->module);
            free(node);
            return;
        case IMPORT:
            mks_free(node->import->name);
            mks_free(node->import->qualified);
            mks_free(node->import->specific);
            free(node->import);
            free(node);
            return;
        case IDENTIFIER:
            free(node->identifier->value);
            free(node->identifier);
            free(node);
            return;
        case NUMBER_LITERAL:
            free(node->number);
            free(node);
            return;
        case FUNCTION_CALL:
            mks_free(node->function_call->name);
            mks_free(node->function_call->arguments);
            free(node->function_call);
            free(node);
            return;
        case FUNCTION:
            mks_free(node->function->arguments);
            mks_free(node->function->body);
            free(node->function);
            free(node);
            return;
        case STRING_LITERAL:
            free(node->string->value);
            free(node->string);
            free(node);
            return;
        case SEQUENCE:
            mks_free(node->sequence->left);
            mks_free(node->sequence->right);
            free(node->sequence);
            free(node);
            return;
        case ASSIGNMENT:
            mks_free(node->assignment->identifier);
            mks_free(node->assignment->value);
            free(node->assignment);
            free(node);
            return;
        case IF_STMT:
            mks_free(node->if_stmt->condition);
            mks_free(node->if_stmt->true_body);
            mks_free(node->if_stmt->false_body);
            free(node->if_stmt);
            free(node);
            return;
        case EQ_OP:
            mks_free(node->eq_op->left);
            mks_free(node->eq_op->right);
            free(node->eq_op);
            free(node);
            return;
        case NE_OP:
            mks_free(node->ne_op->left);
            mks_free(node->ne_op->right);
            free(node->ne_op);
            free(node);
            return;
        case LE_OP:
            mks_free(node->le_op->left);
            mks_free(node->le_op->right);
            free(node->le_op);
            free(node);
            return;
        case GE_OP:
            mks_free(node->ge_op->left);
            mks_free(node->ge_op->right);
            free(node->ge_op);
            free(node);
            return;
        case LT_OP:
            mks_free(node->lt_op->left);
            mks_free(node->lt_op->right);
            free(node->lt_op);
            free(node);
            return;
        case GT_OP:
            mks_free(node->gt_op->left);
            mks_free(node->gt_op->right);
            free(node->gt_op);
            free(node);
            return;
        case PLUS_OP:
            mks_free(node->plus_op->left);
            mks_free(node->plus_op->right);
            free(node->plus_op);
            free(node);
            return;
        case MINUS_OP:
            mks_free(node->minus_op->left);
            mks_free(node->minus_op->right);
            free(node->minus_op);
            free(node);
            return;
        case MULT_OP:
            mks_free(node->mult_op->left);
            mks_free(node->mult_op->right);
            free(node->mult_op);
            free(node);
            return;
        case DIVIDE_OP:
            mks_free(node->divide_op->left);
            mks_free(node->divide_op->right);
            free(node->divide_op);
            free(node);
            return;
        case EMPTY:
            free(node);
            return;
    }
}

mks_node_t *mk_copy(mks_node_t* src) {
    mks_node_t *newnode = mk_node(src->tag);

    switch (src->tag) {
        case IDENTIFIER: {
            mks_identifier_t *id_node = malloc(sizeof(mks_identifier_t));
            id_node->value = strdup(src->identifier->value);
            newnode->identifier = id_node;
            break;
        }
        case IMPORT: {
            mks_import_t *imp_node = malloc(sizeof(mks_import_t));
            imp_node->qualified = mk_copy(src->import->qualified);
            imp_node->name = mk_copy(src->import->name);
            newnode->import = imp_node;
            break;
        }
        default:
            printf("unhandled: %s\n", pretty_stringify_node(src));
            ASSERT(false, "unhandled copy for node");
    }

    return newnode;
}


char *pretty_stringify_node(mks_node_t *node) {
    ASSERT(node != NULL, "handed a null node!");

    char *bfr = NULL;

    switch (node->tag) {
        case MODULE: {
            char *name = pretty_stringify_node(node->module->name);
            char *body = pretty_stringify_node(node->module->body);
            asprintf(&bfr, "<mks_module_t: name=%s body=%s>", name, body);
            free(name);
            free(body);
            return bfr;
        }
        case IMPORT: {
            char *name = pretty_stringify_node(node->import->name);
            char *qual = pretty_stringify_node(node->import->qualified);
            char *specific = pretty_stringify_node(node->import->specific);
            asprintf(&bfr, "<mks_import_t: name=%s qualified=%s specific=%s>", name, qual, specific);
            free(name);
            free(qual);
            free(specific);
            return bfr;
        }
        case IDENTIFIER:
            asprintf(&bfr, "<mks_identifier_t: value=%s>", node->identifier->value);
            return bfr;
        case FUNCTION_CALL: {
            char *name = pretty_stringify_node(node->function_call->name);
            char *args = pretty_stringify_node(node->function_call->arguments);
            asprintf(&bfr, "<mks_function_call_t: name=%s args=%s>", name, args);
            free(name);
            free(args);
            return bfr;
        }
        case FUNCTION: {
            char *args = pretty_stringify_node(node->function->arguments);
            char *body = pretty_stringify_node(node->function->body);
            asprintf(&bfr, "<mks_function_t: args=%s body=%s>", args, body);
            free(body);
            free(args);
            return bfr;
        }
        case NUMBER_LITERAL:
            asprintf(&bfr, "<mks_number_t: value=%i>", node->number->value);
            return bfr;
        case STRING_LITERAL:
            asprintf(&bfr, "<mks_string_t: value=%s>", node->string->value);
            return bfr;
        case SEQUENCE: {
            char *left = pretty_stringify_node(node->sequence->left);
            char *right = pretty_stringify_node(node->sequence->right);
            asprintf(&bfr, "<mks_sequence_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case ASSIGNMENT: {
            char *name = pretty_stringify_node(node->assignment->identifier);
            char *value = pretty_stringify_node(node->assignment->value);
            asprintf(&bfr, "<mks_assignment_t: name=%s, value=%s>", name, value);
            free(name);
            free(value);
            return bfr;
        }
        case IF_STMT: {
            char *condition = pretty_stringify_node(node->if_stmt->condition);
            char *true_body = pretty_stringify_node(node->if_stmt->true_body);
            char *false_body = pretty_stringify_node(node->if_stmt->false_body);
            asprintf(&bfr, "<mks_if_expr_t: condition=%s, true_body=%s, false_body=%s>", condition, true_body,
                     false_body);
            free(condition);
            free(true_body);
            free(false_body);
            return bfr;
        }
        case EQ_OP: {
            char *left = pretty_stringify_node(node->eq_op->left);
            char *right = pretty_stringify_node(node->eq_op->right);
            asprintf(&bfr, "<mk_eq_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case NE_OP: {
            char *left = pretty_stringify_node(node->eq_op->left);
            char *right = pretty_stringify_node(node->eq_op->right);
            asprintf(&bfr, "<mk_ne_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case LE_OP: {
            char *left = pretty_stringify_node(node->le_op->left);
            char *right = pretty_stringify_node(node->le_op->right);
            asprintf(&bfr, "<mk_le_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case GE_OP: {
            char *left = pretty_stringify_node(node->ge_op->left);
            char *right = pretty_stringify_node(node->ge_op->right);
            asprintf(&bfr, "<mk_ge_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case LT_OP: {
            char *left = pretty_stringify_node(node->lt_op->left);
            char *right = pretty_stringify_node(node->lt_op->right);
            asprintf(&bfr, "<mk_lt_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case GT_OP: {
            char *left = pretty_stringify_node(node->gt_op->left);
            char *right = pretty_stringify_node(node->gt_op->right);
            asprintf(&bfr, "<mk_gt_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case PLUS_OP: {
            char *left = pretty_stringify_node(node->plus_op->left);
            char *right = pretty_stringify_node(node->plus_op->right);
            asprintf(&bfr, "<mk_plus_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case MINUS_OP: {
            char *left = pretty_stringify_node(node->minus_op->left);
            char *right = pretty_stringify_node(node->minus_op->right);
            asprintf(&bfr, "<mk_minus_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case MULT_OP: {
            char *left = pretty_stringify_node(node->mult_op->left);
            char *right = pretty_stringify_node(node->mult_op->right);
            asprintf(&bfr, "<mk_mult_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case DIVIDE_OP: {
            char *left = pretty_stringify_node(node->divide_op->left);
            char *right = pretty_stringify_node(node->divide_op->right);
            asprintf(&bfr, "<mk_divide_op_t: left=%s, right=%s>", left, right);
            free(left);
            free(right);
            return bfr;
        }
        case EMPTY: {
            asprintf(&bfr, "<mk_empty>");
            return bfr;
        }
    }

    free(bfr);
    return "Unknown node type";
}

void pretty_print_node(mks_node_t *node) {
    char *seq_str = pretty_stringify_node(node);
    puts(seq_str);
    free(seq_str);
}
