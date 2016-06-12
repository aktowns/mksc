#include "mks_node.h"
#include "utils/log_utils.h"
#include "utils/node_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mks_node.h>

mks_node_t *mk_node(mks_node_type_t tag) {
    mks_node_t *node = malloc(sizeof(mks_node_t));
    node->space = malloc(sizeof(mks_space_t));

    node->tag = tag;
    node->is_ok = true;

    // This shouldn't be hardcoded?
    switch (tag) {
        case NODE_ARRAY_LITERAL:
            node->type = mk_type(UNRESOLVED, UNARY);
            break;
        case NODE_TUPLE:
        case NODE_FUNCTION:
            node->type = mk_type(UNRESOLVED, BINARY);
            break;
        default:
            node->type = mk_type(UNRESOLVED, NULLARY);
    }

    return node;
}

mks_node_t *mk_identifier(char *identifier) {
    ASSERT(identifier != NULL, "handed a null identifier!");

    mks_node_t *node = mk_node(NODE_IDENTIFIER);
    node->identifier = strdup(identifier);

    return node;
}

mks_node_t *mk_module(mks_node_t *identifier, mks_node_t *body, mks_node_t *exports) {
    mks_node_t *node = mk_node(NODE_MODULE);
    mks_module_t *mod_node = malloc(sizeof(mks_module_t));

    mod_node->name = identifier;
    mod_node->body = body;
    mod_node->exports = exports;
    node->module = mod_node;

    return node;
}

mks_node_t *mk_import(mks_node_t *name, mks_node_t *qualified, mks_node_t *specific) {
    mks_node_t *node = mk_node(NODE_IMPORT);
    mks_import_t *imp_node = malloc(sizeof(mks_import_t));

    imp_node->name = name;
    imp_node->qualified = qualified;
    imp_node->specific = specific;
    node->import = imp_node;

    return node;
}

mks_node_t *mk_function_call(mks_node_t *name, mks_node_t *args) {
    mks_node_t *node = mk_node(NODE_FUNCTION_CALL);
    mks_function_call_t *func_call = malloc(sizeof(mks_function_call_t));

    func_call->name = name;
    func_call->arguments = args;
    node->function_call = func_call;

    return node;
}

mks_node_t *mk_function(mks_node_t *arg, mks_node_t *body) {
    mks_node_t *node = mk_node(NODE_FUNCTION);
    mks_function_t *func = malloc(sizeof(mks_function_t));

    func->argument = arg;
    func->body = body;
    node->function = func;

    return node;
}

mks_node_t *mk_string(char *str) {
    ASSERT(str != NULL, "handed a null string!");

    mks_node_t *node = mk_node(NODE_STRING_LITERAL);
    node->string = strdup(str);

    return node;
}

mks_node_t *mk_number(int number) {
    mks_node_t *node = mk_node(NODE_NUMBER_LITERAL);
    node->number = number;

    return node;
}

mks_node_t *mk_boolean(bool boolean) {
    mks_node_t *node = mk_node(NODE_BOOL);
    node->boolean = boolean;

    return node;
}

mks_node_t *mk_array(mks_node_t *items) {
    mks_node_t *node = mk_node(NODE_ARRAY_LITERAL);

    mks_array_t *ary_node = malloc(sizeof(mks_array_t));
    ary_node->items = items;
    node->array = ary_node;

    return node;
}

// WARN: this is a shallow copy
mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right) {
    mks_node_t *node = mk_node(NODE_SEQUENCE);

    mks_sequence_t *seq = malloc(sizeof(mks_sequence_t));
    seq->left = left;
    seq->right = right;
    node->sequence = seq;

    return node;
}

mks_node_t *mk_tuple(mks_node_t *one, mks_node_t *two) {
    mks_node_t *node = mk_node(NODE_TUPLE);

    mks_tuple_t *tup = malloc(sizeof(mks_tuple_t));
    tup->one = one;
    tup->two = two;
    node->tuple = tup;

    return node;
}

mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value) {
    mks_node_t *node = mk_node(NODE_ASSIGNMENT);

    mks_assignment_t *ass = malloc(sizeof(mks_assignment_t));
    ass->name = name;
    ass->value = value;
    node->assignment = ass;

    return node;
}

mks_node_t *mk_if_expr(mks_node_t *condition, mks_node_t *true_body, mks_node_t *false_body) {
    mks_node_t *node = mk_node(NODE_IF);

    mks_if_stmt_t *ifst = malloc(sizeof(mks_if_stmt_t));
    ifst->condition = condition;
    ifst->true_body = true_body;
    ifst->false_body = false_body;
    node->if_stmt = ifst;

    return node;
}

mks_node_t *mk_operator(mks_node_t *left, mks_node_t *right, mks_node_op_type_t op_type) {
    mks_node_t *node = mk_node(NODE_OPERATOR);

    mks_operator_t *op = malloc(sizeof(mks_operator_t));
    op->left = left;
    op->right = right;
    op->op_type = op_type;

    node->operator = op;

    return node;
}

mks_node_t *mk_sequence_to_function_inner(node_list_t *t, mks_node_t *ret) {
    if (t->next == NULL)
        return mk_function(t->node, ret);
    else
        return mk_function(t->node, mk_sequence_to_function_inner(t->next, ret));
}

mks_node_t *mk_sequence_to_function(mks_node_t *arg, mks_node_t *ret) {
    node_list_t *list = NULL;
    mk_seq_to_list(arg, &list, true);

    mks_node_t *nodes = mk_sequence_to_function_inner(list, ret);

    free_node_list(list);

    return nodes;
}

void mks_free_node(mks_node_t *node) {
    if (node == NULL)
        return;

    mks_free_type(node->type);
    free(node->space);
    switch (node->tag) {
        case NODE_MODULE:
            mks_free_node(node->module->name);
            mks_free_node(node->module->body);
            mks_free_node(node->module->exports);
            free(node->module);
            free(node);
            return;
        case NODE_IMPORT:
            mks_free_node(node->import->name);
            mks_free_node(node->import->qualified);
            mks_free_node(node->import->specific);
            free(node->import);
            free(node);
            return;
        case NODE_IDENTIFIER:
            free(node->identifier);
            free(node);
            return;
        case NODE_NUMBER_LITERAL:
            free(node);
            return;
        case NODE_BOOL:
            free(node);
            return;
        case NODE_FUNCTION_CALL:
            mks_free_node(node->function_call->name);
            mks_free_node(node->function_call->arguments);
            free(node->function_call);
            free(node);
            return;
        case NODE_FUNCTION:
            mks_free_node(node->function->argument);
            mks_free_node(node->function->body);
            free(node->function);
            free(node);
            return;
        case NODE_STRING_LITERAL:
            free(node->string);
            free(node);
            return;
        case NODE_ARRAY_LITERAL:
            mks_free_node(node->array->items);
            free(node->array);
            free(node);
            return;
        case NODE_SEQUENCE:
            mks_free_node(node->sequence->left);
            mks_free_node(node->sequence->right);
            free(node->sequence);
            free(node);
            return;
        case NODE_TUPLE:
            mks_free_node(node->tuple->one);
            mks_free_node(node->tuple->two);
            free(node->tuple);
            free(node);
            return;
        case NODE_ASSIGNMENT:
            mks_free_node(node->assignment->name);
            mks_free_node(node->assignment->value);
            free(node->assignment);
            free(node);
            return;
        case NODE_IF:
            mks_free_node(node->if_stmt->condition);
            mks_free_node(node->if_stmt->true_body);
            mks_free_node(node->if_stmt->false_body);
            free(node->if_stmt);
            free(node);
            return;
        case NODE_OPERATOR:
            mks_free_node(node->operator->left);
            mks_free_node(node->operator->right);
            free(node->operator);
            free(node);
            return;
        case NODE_EMPTY:
            free(node);
            return;
    }
}

mks_node_t *mk_copy(mks_node_t *src) {
    mks_node_t *newnode = mk_node(src->tag);

    switch (src->tag) {
        case NODE_IDENTIFIER: {
            newnode->identifier = strdup(src->identifier);
            break;
        }
        case NODE_IMPORT: {
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

void initialize_scoping(mks_node_t *node, mks_node_t *parent) {
    // initialize_scoping(node);
    switch (node->tag) {
        case NODE_MODULE:
            initialize_scoping(node->module->body, node);
            break;
        case NODE_IMPORT:
            /* Hm */
            break;
        case NODE_FUNCTION_CALL:break;
        case NODE_FUNCTION:break;
        case NODE_IDENTIFIER:break;
        case NODE_NUMBER_LITERAL:break;
        case NODE_STRING_LITERAL:
            break;
        case NODE_ARRAY_LITERAL:break;
        case NODE_SEQUENCE:
            initialize_scoping(node->sequence->left, node);
            initialize_scoping(node->sequence->right, node);
            break;
        case NODE_TUPLE:break;
        case NODE_ASSIGNMENT:
            initialize_scoping(node->assignment->value, node);
            break;
        case NODE_IF:
            initialize_scoping(node->if_stmt->condition, node);
            initialize_scoping(node->if_stmt->true_body, node);
            initialize_scoping(node->if_stmt->false_body, node);
            break;
        case NODE_BOOL:break;
        case NODE_EMPTY:break;
        case NODE_OPERATOR:break;
    }
}

char *pretty_stringify_node_op(mks_operator_t *op) {
    ASSERT(op != NULL, "handed a null node!");

    char *bfr = NULL;

    char *left = pretty_stringify_node(op->left);
    char *right = pretty_stringify_node(op->right);

    switch(op->op_type) {
        case OP_EQ:
            asprintf(&bfr, "<(==) %s, %s>", left, right);
            break;
        case OP_NE:
            asprintf(&bfr, "<(!=) %s, %s>", left, right);
            break;
        case OP_LT:
            asprintf(&bfr, "<(lt_op) left=%s right=%s>", left, right);
            break;
        case OP_GT:
            asprintf(&bfr, "<(gt_op) left=%s right=%s>", left, right);
            break;
        case OP_LE:
            asprintf(&bfr, "<(le_op) left=%s right=%s>", left, right);
            break;
        case OP_GE:
            asprintf(&bfr, "<(ge_op) left=%s right=%s>", left, right);
            break;
        case OP_PLUS:
            asprintf(&bfr, "<(+) %s, %s>", left, right);
            break;
        case OP_MINUS:
            asprintf(&bfr, "<(-) %s, %s>", left, right);
            break;
        case OP_MULT:
            asprintf(&bfr, "<(*) %s, %s>", left, right);
            break;
        case OP_DIVIDE:
            asprintf(&bfr, "<(/) %s, %s>", left, right);
            break;
    }

    free(left);
    free(right);
    return bfr;
}

char *pretty_stringify_node(mks_node_t *node) {
    ASSERT(node != NULL, "handed a null node!");

    char *bfr = NULL;
    char *type = pretty_stringify_type(node->type);

    switch (node->tag) {
        case NODE_MODULE: {
            char *name = pretty_stringify_node(node->module->name);
            char *body = pretty_stringify_node(node->module->body);
            char *exports = pretty_stringify_node(node->module->exports);
            asprintf(&bfr, "<(module:%s) name=%s exports=%s body=%s>", type, name, exports, body);
            free(name);
            free(body);
            free(exports);
            break;
        }
        case NODE_IMPORT: {
            char *name = pretty_stringify_node(node->import->name);
            char *qual = pretty_stringify_node(node->import->qualified);
            char *specific = pretty_stringify_node(node->import->specific);
            asprintf(&bfr, "<(import:%s) name=%s qualified=%s specific=%s>", type, name, qual, specific);
            free(name);
            free(qual);
            free(specific);
            break;
        }
        case NODE_IDENTIFIER: {
            asprintf(&bfr, "<(id:%s) %s>", type, node->identifier);
            break;
        }
        case NODE_FUNCTION_CALL: {
            char *name = pretty_stringify_node(node->function_call->name);
            char *args = pretty_stringify_node(node->function_call->arguments);
            asprintf(&bfr, "<(function_call:%s) name=%s args=%s>", type, name, args);
            free(name);
            free(args);
            break;
        }
        case NODE_FUNCTION: {
            char *args = pretty_stringify_node(node->function->argument);
            char *body = pretty_stringify_node(node->function->body);
            asprintf(&bfr, "<(function:%s) args=%s body=%s>", type, args, body);
            free(body);
            free(args);
            break;
        }
        case NODE_NUMBER_LITERAL: {
            asprintf(&bfr, "<(literal:%s) %li>", type, node->number);
            break;
        }
        case NODE_BOOL: {
            asprintf(&bfr, "<(literal:%s) %s>", type, node->boolean ? "true" : "false");
            break;
        }
        case NODE_STRING_LITERAL: {
            asprintf(&bfr, "<(literal:%s) \"%s\">", type, node->string);
            break;
        }
        case NODE_ARRAY_LITERAL: {
            char *items = pretty_stringify_node(node->array->items);
            asprintf(&bfr, "<(literal:%s) [%s]>", type, items);
            free(items);
            break;
        }
        case NODE_SEQUENCE: {
            char *left = pretty_stringify_node(node->sequence->left);
            char *right = pretty_stringify_node(node->sequence->right);
            asprintf(&bfr, "<(sequence:%s) left=%s right=%s>", type, left, right);
            free(left);
            free(right);
            break;
        }
        case NODE_TUPLE: {
            char *one = pretty_stringify_node(node->tuple->one);
            char *two = pretty_stringify_node(node->tuple->two);
            asprintf(&bfr, "<(tuple:%s) %s, %s>", type, one, two);
            free(one);
            free(two);
            break;
        }
        case NODE_ASSIGNMENT: {
            char *name = pretty_stringify_node(node->assignment->name);
            char *value = pretty_stringify_node(node->assignment->value);
            asprintf(&bfr, "<(assignment:%s) %s = %s>", type, name, value);
            free(name);
            free(value);
            break;
        }
        case NODE_IF: {
            char *condition = pretty_stringify_node(node->if_stmt->condition);
            char *true_body = pretty_stringify_node(node->if_stmt->true_body);
            char *false_body = pretty_stringify_node(node->if_stmt->false_body);
            asprintf(&bfr, "<(if:%s) condition=%s true_body=%s false_body=%s>", type, condition, true_body,
                     false_body);
            free(condition);
            free(true_body);
            free(false_body);
            break;
        }
        case NODE_OPERATOR:
            bfr = pretty_stringify_node_op(node->operator);
            break;
        case NODE_EMPTY:
            asprintf(&bfr, "empty");
            break;
    }

    free(type);
    return bfr;
}

char *pretty_stringify_op_value(mks_operator_t *op) {
    ASSERT(op != NULL, "handed a null node!");

    char *bfr = NULL;
    char *left = pretty_stringify_node(op->left);
    char *right = pretty_stringify_node(op->right);

    switch (op->op_type) {
        case OP_EQ:
            asprintf(&bfr, "%s == %s", left, right);
            break;
        case OP_NE:
            asprintf(&bfr, "%s != %s", left, right);
            break;
        case OP_LT:
            asprintf(&bfr, "%s < %s", left, right);
            break;
        case OP_GT:
            asprintf(&bfr, "%s > %s", left, right);
            break;
        case OP_LE:
            asprintf(&bfr, "%s <= %s", left, right);
            break;
        case OP_GE:
            asprintf(&bfr, "%s >= %s", left, right);
            break;
        case OP_PLUS:
            asprintf(&bfr, "%s + %s", left, right);
            break;
        case OP_MINUS:
            asprintf(&bfr, "%s - %s", left, right);
            break;
        case OP_MULT:
            asprintf(&bfr, "%s * %s", left, right);
            break;
        case OP_DIVIDE:
            asprintf(&bfr, "%s / %s", left, right);
            break;
    }

    free(left);
    free(right);
    return bfr;
}

char *pretty_stringify_value(mks_node_t *node) {
    ASSERT(node != NULL, "handed a null node!");

    char *bfr = NULL;

    switch (node->tag) {
        case NODE_MODULE: {
            char *name = pretty_stringify_node(node->module->name);
            asprintf(&bfr, "module %s", name);
            free(name);
            break;
        }
        case NODE_IMPORT: {
            char *name = pretty_stringify_node(node->import->name);
            asprintf(&bfr, "import %s", name);
            free(name);
            break;
        }
        case NODE_IDENTIFIER: {
            asprintf(&bfr, "%s", node->identifier);
            break;
        }
        case NODE_BOOL: {
            asprintf(&bfr, "%s", node->boolean ? "true" : "false");
            break;
        }
        case NODE_FUNCTION_CALL: {
            char *name = pretty_stringify_node(node->function_call->name);
            char *args = pretty_stringify_node(node->function_call->arguments);
            asprintf(&bfr, "%s(%s)", name, args);
            free(name);
            free(args);
            break;
        }
        case NODE_FUNCTION: {
            char *args = pretty_stringify_node(node->function->argument);
            char *body = pretty_stringify_node(node->function->body);
            asprintf(&bfr, "function<%s>(%s)", args, body);
            free(body);
            free(args);
            break;
        }
        case NODE_NUMBER_LITERAL: {
            asprintf(&bfr, "%li", node->number);
            break;
        }
        case NODE_STRING_LITERAL: {
            asprintf(&bfr, "\"%s\"", node->string);
            break;
        }
        case NODE_ARRAY_LITERAL: {
            char *items = pretty_stringify_node(node->array->items);
            asprintf(&bfr, "[%s]", items);
            free(items);
            break;
        }
        case NODE_SEQUENCE: {
            char *left = pretty_stringify_node(node->sequence->left);
            char *right = pretty_stringify_node(node->sequence->right);
            asprintf(&bfr, "(%s, %s)", left, right);
            free(left);
            free(right);
            break;
        }
        case NODE_TUPLE: {
            char *one = pretty_stringify_node(node->tuple->one);
            char *two = pretty_stringify_node(node->tuple->two);
            asprintf(&bfr, "(%s, %s)", one, two);
            free(one);
            free(two);
            break;
        }
        case NODE_ASSIGNMENT: {
            char *name = pretty_stringify_node(node->assignment->name);
            char *value = pretty_stringify_node(node->assignment->value);
            asprintf(&bfr, "%s=%s", name, value);
            free(name);
            free(value);
            break;
        }
        case NODE_IF: {
            char *condition = pretty_stringify_node(node->if_stmt->condition);
            char *true_body = pretty_stringify_node(node->if_stmt->true_body);
            char *false_body = pretty_stringify_node(node->if_stmt->false_body);
            asprintf(&bfr, "if (%s) then { %s } else { %s } fi", condition, true_body, false_body);
            free(condition);
            free(true_body);
            free(false_body);
            break;
        }
        case NODE_OPERATOR:
            bfr = pretty_stringify_op_value(node->operator);
            break;
        case NODE_EMPTY: {
            asprintf(&bfr, "()");
            break;
        }
    }

    return bfr;
}

void pretty_print_node(mks_node_t *node) {
    char *seq_str = pretty_stringify_node(node);
    puts(seq_str);
    free(seq_str);
}
