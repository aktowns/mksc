#ifndef __MKS_TYPE_H
#define __MKS_TYPE_H

struct mks_type;

typedef enum {
    RESOLVED,
    UNRESOLVED
} mks_type_state;

typedef enum {
    NULLARY,
    UNARY,
    BINARY
} mks_type_kind;

typedef enum {
    TY_STRING,
    TY_NUMBER,
    TY_ARRAY,
    TY_BOOL,
    TY_MODULE,
    TY_FUNCTION,
    TY_UNIT
} mks_type_resolution;

struct mks_type;

typedef struct {
    struct mks_type *one;
    struct mks_type *two;
} mks_type_tuple_t;

typedef union {
    struct mks_type* single;
    mks_type_tuple_t* tuple;
} mks_type_value_t;

struct mks_type {
    mks_type_state state;

    mks_type_resolution resolved_type;

    mks_type_kind kind;
    mks_type_value_t *value;
};

typedef struct mks_type mks_type_t;

mks_type_t *mk_type(mks_type_state state, mks_type_kind kind);

char *pretty_stringify_type(mks_type_t *type);

void mks_free_type(mks_type_t *type);

void mks_copy_type(mks_type_t *src, mks_type_t *dst);

#endif