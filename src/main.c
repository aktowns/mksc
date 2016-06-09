#include "mks_node.h"
#include <Block.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  mks_node_t *identifier = mk_identifier("Hello World");
  mks_node_t *somevar = mk_identifier("some_var");
  mks_node_t *number = mk_number(1337);
  mks_node_t *assvar = mk_assignment(somevar, number);
  mks_node_t *identifier_bye = mk_identifier("Goodbye Cruel World");

  mks_node_t *seq = mk_sequence(mk_sequence(identifier, assvar), identifier_bye);

  char *seq_str = pretty_print_node(seq);
  printf("seq=%s\n", seq_str);

  // char* id_str = pretty_print_node(identifier);
  // char* num_str = pretty_print_node(number);
  // printf("identifier=%s, number=%s\n", id_str, num_str);
  // free(id_str);
  // free(num_str);
  
  free(seq_str);

  mks_free(seq); // free's children

  void (^hello)(void) = ^(void) {
    printf("Hello, block!\n");
  };
  hello();

  return 0;
}
