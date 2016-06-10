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

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

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

const char* string_token(token_t* token) {
  switch (token->type) {
    case NUMBER: return "NUMBER";
    case PLUS: return "PLUS";
    case SEMICOLON: return "SEMICOLON";
    case ASSIGN: return "ASSIGN";
    case LPAREN: return "LPAREN";
    case RPAREN: return "RPAREN";
    case IDENTIFIER: return "IDENTIFIER";
    case EQ: return "EQ";
    case IF: return "IF";
    case ELSE: return "ELSE";
    case THEN: return "THEN";
    case FI: return "FI";
    case MINUS: return "MINUS";
    case DIVIDE: return "DIVIDE";
    case TIMES: return "TIMES";
    case NE: return "NE";
    case LT: return "LT";
    case LE: return "LE";
    case GT: return "GT";
    case GE: return "GE";
    case DO: return "DO";
    case OD: return "OD";
    case MULT: return "MULT";
    default:
      printf("Unknown token type: %i\n", token->type);
      return "unknown";
  }
}

void print_tokens(token_t* tokens) {
  printf("%sTOKEN_TREE%s: ", KCYN, KNRM);
  do {
    printf("-> %s(%i:%i) ", string_token(tokens), tokens->line_no, tokens->column_no);
  } while ((tokens = tokens->next) != NULL);
  printf("\n");
}

void annotate_source(char* path, token_t* tokens) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(path, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int lineno = 1;
    printf("%s== Annotating: %s ==%s\n", KRED, path, KNRM);
    while ((read = getline(&line, &len, fp)) != -1) {
      printf("%s%i%s: %s", KGRN, lineno, KNRM, line);
      printf("%s%i%s: ", KBLU, lineno, KNRM);
      token_t* toks = tokens;
      do {
        if (toks->line_no == lineno) {
          printf("%s ", string_token(toks));
        }
      } while ((toks = toks->next) != NULL);
      printf("\n");
      lineno += 1;
    }
    printf("%s====%s\n", KRED, KNRM);

    fclose(fp);
    if (line)
        free(line);
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
  annotate_source(argv[1], tokens);

  void* parser = ParseAlloc((void*)malloc);

  do {
    Parse(parser, cur_token->type, cur_token, NULL);
  } while ((cur_token = cur_token->next) != NULL);

  lex_free(tokens);

  mks_node_t* ast = NULL;
  Parse(parser, 0, NULL, &ast);

  if (ast != NULL) {
    printf("%sAST%s: ", KCYN, KNRM);
    pretty_print_node(ast);
    mks_free(ast);
  }

  ParseFree(parser, (void*)free);

  free(contents);

  return 0;
}
