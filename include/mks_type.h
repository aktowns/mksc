#ifndef __MKS_TYPE_H
#define __MKS_TYPE_H

struct mks_type;

typedef enum {
    RESOLVED,
    UNRESOLVED
} mks_type_state;

typedef enum {
    NULLARY,
    UNARY
} mks_type_kind;

typedef enum {
    TY_STRING,
    TY_NUMBER,
    TY_ARRAY,
    TY_BOOL,
    TY_MODULE,
    TY_UNIT
} mks_type_resolution;

struct mks_type {
    mks_type_state state;

    mks_type_resolution resolved_type;
    // mks_type_kind kind;

    struct mks_type *contained_type;
};

typedef struct mks_type mks_type_t;

mks_type_t *mk_type(mks_type_state state);

char *pretty_stringify_type(mks_type_t *type);

void mks_free_type(mks_type_t *type);

void mks_copy_type(mks_type_t *src, mks_type_t *dst);

#endif