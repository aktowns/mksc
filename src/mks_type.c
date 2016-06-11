#include <stdlib.h>
#include <stdio.h>

#include "utils/ansi_colour.h"
#include "utils/log_utils.h"
#include "mks_type.h"

mks_type_t *mk_type(mks_type_state state, mks_type_kind kind) {
    mks_type_t *typ = malloc(sizeof(mks_type_t));

    typ->state = state;
    typ->kind = kind;
    typ->value = malloc(sizeof(mks_type_value_t));

    if (typ->kind == UNARY) {
        typ->value->single = mk_type(UNRESOLVED, NULLARY);
    } else if (typ->kind == BINARY) {
        typ->value->tuple = malloc(sizeof(mks_type_tuple_t));
        typ->value->tuple->one = mk_type(UNRESOLVED, NULLARY);
        typ->value->tuple->two = mk_type(UNRESOLVED, NULLARY);
    }

    return typ;
}

void mks_copy_type(mks_type_t *src, mks_type_t *dst) {
    ASSERT(src != NULL, "copy handed a null source");
    ASSERT(dst != NULL, "copy handed a null destination");

    dst->state = src->state;
    dst->resolved_type = src->resolved_type;

    if (src->kind == UNARY) {
        if (dst->value == NULL) {
            dst->value = malloc(sizeof(mks_type_value_t));
        } else {
            if (dst->kind == BINARY && dst->value->tuple != NULL) {
                if (dst->value->tuple->one != NULL)
                    mks_free_type(dst->value->tuple->one);
                if (dst->value->tuple->two != NULL)
                    mks_free_type(dst->value->tuple->two);

                free(dst->value->tuple);
            } else if (dst->kind == UNARY) {
                mks_free_type(dst->value->single);
            }
        }

        dst->value->single = mk_type(UNRESOLVED, NULLARY);

        if (src->value->single != NULL && src->value->single->state == RESOLVED) {
            if (dst->value->single == NULL) {
                dst->value->single = mk_type(UNRESOLVED, NULLARY);
            }

            mks_copy_type(src->value->single, dst->value->single);
        }
    } else if (src->kind == BINARY) {
        if (dst->value == NULL) {
            dst->value = malloc(sizeof(mks_type_value_t));
        } else {
            if (dst->kind == BINARY && dst->value->tuple != NULL) {
                if (dst->value->tuple->one != NULL)
                    mks_free_type(dst->value->tuple->one);
                if (dst->value->tuple->two != NULL)
                    mks_free_type(dst->value->tuple->two);

                free(dst->value->tuple);
            } else if (dst->kind == UNARY) {
                mks_free_type(dst->value->single);
            }
        }

        dst->value->tuple = malloc(sizeof(mks_type_tuple_t));
        dst->value->tuple->one = mk_type(UNRESOLVED, NULLARY);
        dst->value->tuple->two = mk_type(UNRESOLVED, NULLARY);

        if (src->value->tuple->one != NULL && src->value->tuple->one->state == RESOLVED) {
            mks_copy_type(src->value->tuple->one, dst->value->tuple->one);
        }
        if (src->value->tuple->two != NULL && src->value->tuple->two->state == RESOLVED) {
            mks_copy_type(src->value->tuple->two, dst->value->tuple->two);
        }
    }

    dst->kind = src->kind;
}

void mks_free_type(mks_type_t *type) {
    if (type == NULL)
        return;

    if (type->kind == UNARY) {
        mks_free_type(type->value->single);
    } else if (type->kind == BINARY) {
        mks_free_type(type->value->tuple->one);
        mks_free_type(type->value->tuple->two);
        free(type->value->tuple);
    }

    free(type->value);
    free(type);
}

char *pretty_stringify_type(mks_type_t *type) {
    ASSERT(type != NULL, "handed a null type!");

    char *bfr = NULL;

    if (type->state == UNRESOLVED) {
        asprintf(&bfr, "%sunresolved%s", RGB(225, 60, 60), ANSI_NORMAL);
        return bfr;
    }

    switch (type->resolved_type) {
        case TY_STRING:
            asprintf(&bfr, "%sstring%s", RGB(100, 200, 100), ANSI_NORMAL);
            break;
        case TY_NUMBER:
            asprintf(&bfr, "%snumber%s", RGB(200, 100, 100), ANSI_NORMAL);
            break;
        case TY_ARRAY: {
            if (type->value->single->state == RESOLVED) {
                char *contained = pretty_stringify_type(type->value->single);
                asprintf(&bfr, "%sarray%s<%s>", RGB(150, 200, 50), ANSI_NORMAL, contained);
                free(contained);
            } else {
                asprintf(&bfr, "%sarray%s<?>", RGB(250, 100, 50), ANSI_NORMAL);
            }
            break;
        }
        case TY_UNIT:
            asprintf(&bfr, "%sunit%s", RGB(200, 255, 200), ANSI_NORMAL);
            break;
        case TY_MODULE:
            asprintf(&bfr, "%smodule%s", RGB(80, 80, 240), ANSI_NORMAL);
            break;
        case TY_BOOL:
            asprintf(&bfr, "%sbool%s", RGB(200, 80, 240), ANSI_NORMAL);
            break;
        case TY_FUNCTION: {
            char *arg = pretty_stringify_type(type->value->tuple->one);
            char *ret = pretty_stringify_type(type->value->tuple->two);
            asprintf(&bfr, "%sfunction%s<%s,%s>", RGB(255, 165, 20), ANSI_NORMAL, arg, ret);
            free(arg);
            free(ret);
            break;
        }
    }

    return bfr;
}