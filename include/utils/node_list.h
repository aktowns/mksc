#ifndef __NODE_LIST_H
#define __NODE_LIST_H

#include <mks_node.h>

typedef struct nodelist {
    mks_node_t *node;
    struct nodelist *previous;
    struct nodelist *next;
} node_list_t;

typedef void (^node_list_iterate_cb)(node_list_t *);

node_list_t* mk_seq_to_list(mks_node_t* seq, node_list_t *c, node_list_t *t, bool free_seq);

void node_list_iterate(node_list_t *list_top, node_list_iterate_cb cb);

void free_node_list(node_list_t *nodes);

#endif