#include "mks_node.h"
#include "token.h"
#include "parser.h"

#include <Block.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void *ParseAlloc(void *);
void Parse(void *, int, void*, void*);
void ParseFree(void *, void *);

token_t* lex(char* bfr);
void lex_free(token_t* tokens);

char* read_file(char* path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    printf("failed to read file\n");
    abort();
  }

  fseek(f, 0, SEEK_END);
  int length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(length + 1);
  if (!buffer) {
    printf("failed to allocate space for the file");
    abort();
  }
  fread(buffer, 1, length, f);
  fclose(f);

  buffer[length] = '\0';

  return buffer;
}

const char* print_token(token_t* token) {
  switch (token->type) {
    case NUMBER: return "NUMBER";
    case PLUS: return "PLUS";
    case SEMICOLON: return "SEMICOLON";
    case ASSIGN: return "ASSIGN";
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case NAME: return "NAME";
    case EQ: return "EQ";
    case IF: return "IF";
    case ELSE: return "ELSE";
    case THEN: return "THEN";
    case FI: return "FI";
    default:
      printf("Unknown token type: %i\n", token->type);
      return "unknown";
  }
}

void print_tokens(token_t* tokens) {
  printf("TOKEN_TREE: ");
  do {
    printf("-> %s ", print_token(tokens));
  } while ((tokens = tokens->next) != NULL);
  printf("\n");
}


int main(int argc, char** argv) {
  if((argc < 2) || access(argv[1], F_OK) == -1) {
    printf("usage: %s <filename>\n", argv[0]);
    exit(-1);
  }

  char* contents = read_file(argv[1]);

  token_t *tokens = lex(contents);
  token_t *cur_token = tokens;

  print_tokens(tokens);

  void* parser = ParseAlloc((void*)malloc);

  do {
    Parse(parser, cur_token->type, cur_token, NULL);
  } while ((cur_token = cur_token->next) != NULL);

  lex_free(tokens);

  mks_node_t* ast = NULL;
  Parse(parser, 0, NULL, &ast);

  if (ast != NULL) {
    pretty_print_node(ast);
    mks_free(ast);
  }

  ParseFree(parser, (void*)free);

  free(contents);

  return 0;
}
