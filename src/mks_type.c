#include <stdlib.h>
#include <stdio.h>
#include <utils/ansi_colour.h>
#include "utils/log_utils.h"
#include "mks_type.h"

mks_type_t *mk_type(mks_type_state state) {
    mks_type_t *typ = malloc(sizeof(mks_type_t));

    typ->state = state;

    typ->contained_type = malloc(sizeof(mks_type_t));
    typ->contained_type->state = UNRESOLVED;
    typ->contained_type->contained_type = NULL;

    // typ->kind = kind;

    //    if (kind == UNARY) {
    //        typ->contained_type = malloc(sizeof(mks_type_t));
    //        typ->contained_type->state = UNRESOLVED;
    //    }

    return typ;
}

void mks_copy_type(mks_type_t *src, mks_type_t *dst) {
    ASSERT(src != NULL, "copy handed a null source");
    ASSERT(dst != NULL, "copy handed a null destination");

    dst->state = src->state;
    dst->resolved_type = src->resolved_type;

    if (src->contained_type != NULL && src->contained_type->state == RESOLVED) {
        if (dst->contained_type == NULL) {
            dst->contained_type = mk_type(UNRESOLVED);
        }

        mks_copy_type(src->contained_type, dst->contained_type);
    }
}

void mks_free_type(mks_type_t *type) {
    if (type == NULL)
        return;

    if (type->contained_type != NULL) {
        mks_free_type(type->contained_type);
    }

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
            if (type->contained_type->state == RESOLVED) {
                char *contained = pretty_stringify_type(type->contained_type);
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
    }

    return bfr;
}