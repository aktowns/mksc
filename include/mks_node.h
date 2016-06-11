#ifndef __MKS_NODE_H
#define __MKS_NODE_H

#include <stdbool.h>

struct mks_node;

typedef enum {
    MODULE,
    IMPORT,
    FUNCTION_CALL,
    FUNCTION,
    IDENTIFIER,
    NUMBER_LITERAL,
    STRING_LITERAL,
    SEQUENCE,
    ASSIGNMENT,
    IF,
    EQ_OP,
    NE_OP,
    LT_OP,
    GT_OP,
    LE_OP,
    GE_OP,
    PLUS_OP,
    MINUS_OP,
    MULT_OP,
    DIVIDE_OP,
    EMPTY
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
    struct mks_node *left;
    struct mks_node *right;
} mks_sequence_t;

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
} mks_eq_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_ne_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_lt_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_gt_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_le_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_ge_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_plus_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_minus_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_mult_operator_t;

typedef struct {
    struct mks_node *left;
    struct mks_node *right;
} mks_divide_operator_t;

typedef struct {
    struct mks_node *name;
    struct mks_node *arguments;
} mks_function_call_t;

typedef struct {
    struct mks_node *arguments;
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

    union {
        mks_identifier_t *identifier;
        mks_number_t *number;
        mks_string_t *string;
        mks_sequence_t *sequence;
        mks_assignment_t *assignment;
        mks_if_stmt_t *if_stmt;
        mks_eq_operator_t *eq_op;
        mks_ne_operator_t *ne_op;
        mks_lt_operator_t *lt_op;
        mks_gt_operator_t *gt_op;
        mks_le_operator_t *le_op;
        mks_ge_operator_t *ge_op;
        mks_plus_operator_t *plus_op;
        mks_minus_operator_t *minus_op;
        mks_mult_operator_t *mult_op;
        mks_divide_operator_t *divide_op;
        mks_module_t *module;
        mks_function_call_t *function_call;
        mks_function_t *function;
        mks_import_t *import;
        char* error;
    };
};

typedef struct mks_node mks_node_t;

mks_node_t *mk_node(mks_node_type tag);

mks_node_t *mk_copy(mks_node_t* src);

mks_node_t *mk_module(mks_node_t *identifier, mks_node_t *body, mks_node_t* exports);

mks_node_t *mk_import(mks_node_t *name, mks_node_t *qualified, mks_node_t *specific);

mks_node_t *mk_function_call(mks_node_t *name, mks_node_t *args);

mks_node_t *mk_function(mks_node_t *args, mks_node_t *body);

mks_node_t *mk_identifier(char *value);

mks_node_t *mk_string(char *identifier);

mks_node_t *mk_number(int value);

mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right);

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

void mks_free(mks_node_t *node);

char *pretty_stringify_node(mks_node_t *node);

void pretty_print_node(mks_node_t *node);

#endif
