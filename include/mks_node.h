#ifndef __MKS_NODE_H
#define __MKS_NODE_H

#include <stdbool.h>
#include <unistring/stdbool.h>
#include "mks_type.h"

struct mks_node;

typedef enum {
    NODE_MODULE,
    NODE_IMPORT,
    NODE_FUNCTION_CALL,
    NODE_FUNCTION,
    NODE_IDENTIFIER,
    NODE_NUMBER_LITERAL,
    NODE_STRING_LITERAL,
    NODE_ARRAY_LITERAL,
    NODE_SEQUENCE,
    NODE_TUPLE,
    NODE_ASSIGNMENT,
    NODE_IF,
    NODE_EQ_OP,
    NODE_NE_OP,
    NODE_LT_OP,
    NODE_GT_OP,
    NODE_LE_OP,
    NODE_GE_OP,
    NODE_PLUS_OP,
    NODE_MINUS_OP,
    NODE_MULT_OP,
    NODE_DIVIDE_OP,
    NODE_EMPTY
} mks_node_type;

typedef struct {
    char *value;
} mks_identifier_t;

typedef struct {
    struct mks_node *name;
    struct mks_node *body;
    struct mks_node *exports;
} mks_module_t;

typedef struct {
    int value;
} mks_number_t;

typedef struct {
    char *value;
} mks_string_t;

typedef struct {
    struct mks_node *items;
} mks_array_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_sequence_t;

typedef struct {
    struct mks_node *one;
    struct mks_node *two;
} mks_tuple_t;

typedef struct {
    struct mks_node *identifier;
    struct mks_node *value;
} mks_assignment_t;

typedef struct {
    struct mks_node *condition;
    struct mks_node *true_body;
    struct mks_node *false_body;
} mks_if_stmt_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_operator_t;

typedef struct {
    struct mks_node *name;
    struct mks_node *arguments;
} mks_function_call_t;

typedef struct {
    struct mks_node *argument;
    struct mks_node *body;
} mks_function_t;

typedef struct {
    struct mks_node *name;
    struct mks_node *qualified;
    struct mks_node *specific;
} mks_import_t;

struct mks_node {
    mks_node_type tag;
    bool is_ok;
    mks_type_t *type;

    union {
        mks_identifier_t *identifier;
        mks_number_t *number;
        mks_string_t *string;
        mks_array_t *array;
        mks_sequence_t *sequence;
        mks_tuple_t *tuple;
        mks_assignment_t *assignment;
        mks_if_stmt_t *if_stmt;
        mks_operator_t *eq_op;
        mks_operator_t *ne_op;
        mks_operator_t *lt_op;
        mks_operator_t *gt_op;
        mks_operator_t *le_op;
        mks_operator_t *ge_op;
        mks_operator_t *plus_op;
        mks_operator_t *minus_op;
        mks_operator_t *mult_op;
        mks_operator_t *divide_op;
        mks_module_t *module;
        mks_function_call_t *function_call;
        mks_function_t *function;
        mks_import_t *import;
        char *error;
    };
};

typedef struct mks_node mks_node_t;

mks_node_t *mk_node(mks_node_type tag);

mks_node_t *mk_copy(mks_node_t *src);

mks_node_t *mk_module(mks_node_t *identifier, mks_node_t *body, mks_node_t *exports);

mks_node_t *mk_import(mks_node_t *name, mks_node_t *qualified, mks_node_t *specific);

mks_node_t *mk_function_call(mks_node_t *name, mks_node_t *args);

mks_node_t *mk_function(mks_node_t *args, mks_node_t *body);

mks_node_t *mk_identifier(char *value);

mks_node_t *mk_string(char *identifier);

mks_node_t *mk_number(int value);

mks_node_t *mk_array(mks_node_t *items);

mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_tuple(mks_node_t *one, mks_node_t *two);

mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value);

mks_node_t *mk_if_expr(mks_node_t *condition, mks_node_t *true_body, mks_node_t *false_body);

mks_node_t *mk_eq_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_ne_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_lt_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_gt_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_le_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_ge_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_plus_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_minus_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_mult_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_divide_operator(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_sequence_to_function(mks_node_t *arg, mks_node_t *ret);

void mks_free_node(mks_node_t *node);

char *pretty_stringify_node(mks_node_t *node);

char *pretty_stringify_value(mks_node_t *node);

void pretty_print_node(mks_node_t *node);

#endif
