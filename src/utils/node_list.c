#include "utils/node_list.h"
#include "mks_node.h"

#include <stdlib.h>

node_list_t* mk_seq_to_list(mks_node_t* seq, node_list_t *c, node_list_t *t, bool free_seq) {
    t->previous = c;
    t->next = NULL;

    if (seq->tag == NODE_SEQUENCE) {
        t->node = seq->sequence->right;
        mks_node_t* left = seq->sequence->left;

        if (free_seq) {
            mks_free_type(seq->type);
            free(seq->sequence);
            free(seq);
        }

        t->next = malloc(sizeof(node_list_t));
        return mk_seq_to_list(left, t, t->next, free_seq);
    }

    t->node = seq;

    return t;
}

void node_list_iterate(node_list_t *list_top, node_list_iterate_cb cb) {
    do {
        cb(list_top);
    } while ((list_top = list_top->next) != NULL);
}

void free_node_list(node_list_t *nodes) {
    node_list_t *curr;
    while ((curr = nodes) != NULL) {
        nodes = nodes->next;
        free(curr);
    }
}