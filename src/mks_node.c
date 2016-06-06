#include <mks_node.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mks_node_t *mk_node(mks_node_type tag) {
  mks_node_t *node = malloc(sizeof(mks_node_t));

  node->tag = tag;

  return node;
}

mks_node_t *mk_identifier(char *identifier) {
  if (identifier == NULL) {
    return NULL;
  }

  mks_node_t *node = mk_node(IDENTIFIER);
  mks_identifier_t *id_node = malloc(sizeof(mks_identifier_t));

  id_node->value = strdup(identifier);
  node->identifier = id_node;

  return node;
}

mks_node_t *mk_number(int number) {
  mks_node_t *node = mk_node(NUMBER);

  mks_number_t *num_node = malloc(sizeof(mks_number_t));
  num_node->value = number;
  node->number = num_node;

  return node;
}

// WARN: this is a shallow copy
mks_node_t *mk_sequence(mks_node_t *left, mks_node_t *right) {
  mks_node_t *node = mk_node(SEQUENCE);

  mks_sequence_t *seq = malloc(sizeof(mks_sequence_t));
  seq->left = left;
  seq->right = right;
  node->sequence = seq;

  return node;
}

mks_node_t *mk_assignment(mks_node_t *name, mks_node_t *value) {
  mks_node_t *node = mk_node(ASSIGNMENT);

  mks_assignment_t *ass = malloc(sizeof(mks_assignment_t));
  ass->identifier = name;
  ass->value = value;
  node->assignment = ass;

  return node;
}

mks_node_t *mk_if_stmt(mks_node_t *condition, mks_node_t *true_body,
                       mks_node_t *false_body) {
  mks_node_t *node = mk_node(IF_STMT);

  mks_if_stmt_t *ifst = malloc(sizeof(mks_if_stmt_t));
  ifst->condition = condition;
  ifst->true_body = true_body;
  ifst->false_body = false_body;
  node->if_stmt = ifst;

  return node;
}

mks_node_t *mk_while_stmt(mks_node_t *condition, mks_node_t *body) {
  mks_node_t *node = mk_node(WHILE_STMT);

  mks_while_stmt_t *whilest = malloc(sizeof(mks_while_stmt_t));
  whilest->condition = condition;
  whilest->body = body;
  node->while_stmt = whilest;

  return node;
}

mks_node_t *mk_eq_operator(mks_node_t *left, mks_node_t *right) {
  mks_node_t *node = mk_node(EQ_OP);

  mks_eq_operator_t *eq = malloc(sizeof(mks_eq_operator_t));
  eq->left = left;
  eq->right = right;
  node->eq_op = eq;

  return node;
}

mks_node_t *mk_ne_operator(mks_node_t *left, mks_node_t *right) {
  mks_node_t *node = mk_node(NE_OP);

  mks_ne_operator_t *ne = malloc(sizeof(mks_ne_operator_t));
  ne->left = left;
  ne->right = right;
  node->ne_op = ne;

  return node;
}

void mks_free(mks_node_t *node) {
  if (node == NULL)
    return;

  switch (node->tag) {
  case IDENTIFIER:
    free(node->identifier->value);
    free(node->identifier);
    free(node);
    return;
  case NUMBER:
    free(node->number);
    free(node);
    return;
  case SEQUENCE:
    mks_free(node->sequence->left);
    mks_free(node->sequence->right);
    free(node->sequence);
    free(node);
    return;
  case ASSIGNMENT:
    mks_free(node->assignment->identifier);
    mks_free(node->assignment->value);
    free(node->assignment);
    free(node);
    return;
  case IF_STMT:
    mks_free(node->if_stmt->condition);
    mks_free(node->if_stmt->true_body);
    mks_free(node->if_stmt->false_body);
    free(node->if_stmt);
    free(node);
    return;
  case WHILE_STMT:
    mks_free(node->while_stmt->condition);
    mks_free(node->while_stmt->body);
    free(node->while_stmt);
    free(node);
    return;
  case EQ_OP:
    mks_free(node->eq_op->left);
    mks_free(node->eq_op->right);
    free(node->eq_op);
    free(node);
    return;
  case NE_OP:
    mks_free(node->eq_op->left);
    mks_free(node->ne_op->right);
    free(node->ne_op);
    free(node);
    return;
  }
}

char *pretty_print_node(mks_node_t *node) {
  char *bfr = NULL;

  switch (node->tag) {
  case IDENTIFIER:
    asprintf(&bfr, "<mks_identifier_t: value=%s>", node->identifier->value);
    return bfr;
  case NUMBER:
    asprintf(&bfr, "<mks_number_t: value=%i", node->number->value);
    return bfr;
  case SEQUENCE: {
    char *left = pretty_print_node(node->sequence->left);
    char *right = pretty_print_node(node->sequence->right);
    asprintf(&bfr, "<mks_sequence_t: left=%s, right=%s>", left, right);
    free(left);
    free(right);
    return bfr;
  }
  case ASSIGNMENT: {
    char *name = pretty_print_node(node->assignment->identifier);
    char *value = pretty_print_node(node->assignment->value);
    asprintf(&bfr, "<mks_assignment_t: name=%s, value=%s>", name, value);
    free(name);
    free(value);
    return bfr;
  }
  case IF_STMT: {
    char *condition = pretty_print_node(node->if_stmt->condition);
    char *true_body = pretty_print_node(node->if_stmt->true_body);
    char *false_body = pretty_print_node(node->if_stmt->false_body);
    asprintf(&bfr,
             "<mks_if_smtmt_t: condition=%s, true_body=%s, false_body=%s>",
             condition, true_body, false_body);
    free(condition);
    free(true_body);
    free(false_body);
    return bfr;
  }
  case WHILE_STMT: {
    char *condition = pretty_print_node(node->while_stmt->condition);
    char *body = pretty_print_node(node->while_stmt->body);
    asprintf(&bfr, "<mk_while_stmt_t: condition=%s, body=%s>", condition, body);
    free(condition);
    free(body);
    return bfr;
  }
  case EQ_OP: {
    char *left = pretty_print_node(node->eq_op->left);
    char *right = pretty_print_node(node->eq_op->right);
    asprintf(&bfr, "<mk_eq_op_t: left=%s, right=%s>", left, right);
    free(left);
    free(right);
    return bfr;
  }
  case NE_OP: {
    char *left = pretty_print_node(node->eq_op->left);
    char *right = pretty_print_node(node->eq_op->right);
    asprintf(&bfr, "<mk_ne_op_t: left=%s, right=%s>", left, right);
    free(left);
    free(right);
    return bfr;
  }
  default:
    free(bfr);
    return "Unknown node type";
  }
}
