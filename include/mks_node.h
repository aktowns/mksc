#ifndef __MKS_NODE_H
#define __MKS_NODE_H

struct mks_node;

typedef enum {
  IDENTIFIER,
  NUMBER,
  SEQUENCE,
  ASSIGNMENT,
  IF_STMT,
  WHILE_STMT,
  EQ_OP,
  NE_OP
} mks_node_type;

typedef struct { char *value; } mks_identifier_t;

typedef struct { int value; } mks_number_t;

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
  struct mks_node *condition;
  struct mks_node *body;
} mks_while_stmt_t;

typedef struct {
  struct mks_node *left;
  struct mks_node *right;
} mks_eq_operator_t;

typedef struct {
  struct mks_node *left;
  struct mks_node *right;
} mks_ne_operator_t;

struct mks_node {
  mks_node_type tag;

  union {
    mks_identifier_t *identifier;
    mks_number_t *number;
    mks_sequence_t *sequence;
    mks_assignment_t *assignment;
    mks_if_stmt_t *if_stmt;
    mks_while_stmt_t *while_stmt;
    mks_eq_operator_t *eq_op;
    mks_ne_operator_t *ne_op;
  };
};

typedef struct mks_node mks_node_t;

mks_node_t *mk_identifier(char *value);
mks_node_t *mk_number(int value);
mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right);
mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value);
mks_node_t *mk_if_stmt(mks_node_t *condition, mks_node_t *true_body,
                       mks_node_t *false_body);
mks_node_t *mk_while_smtt(mks_node_t *condition, mks_node_t *body);
mks_node_t *mk_eq_op(mks_node_t *left, mks_node_t *right);
mks_node_t *mk_ne_op(mks_node_t *left, mks_node_t *right);

void mks_free(mks_node_t *node);
char *pretty_print_node(mks_node_t *node);

#endif
