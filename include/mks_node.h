#ifndef __MKS_NODE_H
#define __MKS_NODE_H

#include <stdbool.h>
#include <stdint.h>
#include <unistring/stdbool.h>
#include <uthash.h>
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
    NODE_BOOL,
    NODE_OPERATOR,
    NODE_EMPTY
} mks_node_type_t;

typedef enum {
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_PLUS,
    OP_MINUS,
    OP_MULT,
    OP_DIVIDE
} mks_node_op_type_t;

typedef struct {
    struct mks_node *name;
    struct mks_node *body;
    struct mks_node *exports;
} mks_module_t;

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
    struct mks_node *name;
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
    mks_node_op_type_t op_type;
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

typedef struct {
    char *id;
    struct mks_node *ref;
    UT_hash_handle hh;
} mks_space_t;

struct mks_node {
    mks_node_type_t tag;
    bool is_ok;
    mks_type_t *type;
    mks_space_t *space;

    union {
        char *identifier;
        int64_t number;
        char *string;
        bool boolean;

        mks_array_t *array;
        mks_sequence_t *sequence;
        mks_tuple_t *tuple;
        mks_assignment_t *assignment;
        mks_if_stmt_t *if_stmt;
        mks_operator_t *operator;
        mks_module_t *module;
        mks_function_call_t *function_call;
        mks_function_t *function;
        mks_import_t *import;
        char *error;
    };
};

typedef struct mks_node mks_node_t;

mks_node_t *mk_node(mks_node_type_t tag);

mks_node_t *mk_copy(mks_node_t *src);

mks_node_t *mk_module(mks_node_t *identifier, mks_node_t *body, mks_node_t *exports);

mks_node_t *mk_import(mks_node_t *name, mks_node_t *qualified, mks_node_t *specific);

mks_node_t *mk_function_call(mks_node_t *name, mks_node_t *args);

mks_node_t *mk_function(mks_node_t *args, mks_node_t *body);

mks_node_t *mk_identifier(char *value);

mks_node_t *mk_string(char *identifier);

mks_node_t *mk_number(int value);

mks_node_t *mk_boolean(bool boolean);

mks_node_t *mk_array(mks_node_t *items);

mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right);

mks_node_t *mk_tuple(mks_node_t *one, mks_node_t *two);

mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value);

mks_node_t *mk_if_expr(mks_node_t *condition, mks_node_t *true_body, mks_node_t *false_body);

mks_node_t *mk_operator(mks_node_t *left, mks_node_t *right, mks_node_op_type_t op_type);

mks_node_t *mk_sequence_to_function(mks_node_t *arg, mks_node_t *ret);

void initialize_scoping(mks_node_t *node, mks_node_t *parent);

void mks_free_node(mks_node_t *node);

char *pretty_stringify_node(mks_node_t *node);

char *pretty_stringify_value(mks_node_t *node);

void pretty_print_node(mks_node_t *node);

void report_exports(mks_node_t* node);

void mks_free_space(mks_space_t* node);

#endif
