#ifndef __TOKEN_H
#define __TOKEN_H

typedef struct node {
  int type;
  union {
    long int number_value;
    char* string_value;
  };
  struct node *next;
} token_t;

#endif
