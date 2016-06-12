#include "utils/node_list.h"

#include <stdlib.h>
#include <utlist.h>

// keeps the pointers dangling
void free_sequence(mks_node_t *seq) {
    mks_free_type(seq->type);
    free(seq->sequence);
    free(seq);
}

void mk_seq_to_list(mks_node_t *seq, node_list_t **dest, bool free_seq) {
    node_list_t *list = malloc(sizeof(node_list_t));

    if (seq->tag == NODE_SEQUENCE) {
        mks_node_t *left = seq->sequence->left;
        mks_node_t *right = seq->sequence->right;

        if (free_seq) free_sequence(seq);

        list->node = right;

        DL_PREPEND(*dest, list);

        return mk_seq_to_list(left, dest, free_seq);
    }

    list->node = seq;
    DL_PREPEND(*dest, list);
}

void node_list_iterate(node_list_t *list_top, node_list_iterate_cb cb) {
    node_list_t *elt, *tmp;
    DL_FOREACH_SAFE(list_top, elt, tmp) {
        cb(elt);
    }
}

void free_node_list(node_list_t *list_top) {
    node_list_t *elt, *tmp;
    DL_FOREACH_SAFE(list_top, elt, tmp) {
        free(elt);
    }
}
