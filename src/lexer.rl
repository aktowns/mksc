#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <stdint.h>
#include <string_utils.h>
#include <token.h>

%%{
  machine mks_lexer;

#	newline = '\n' @{curline += 1;};

  number = ('+'|'-')?[0-9]+;
  alpha_u = alpha | '_';

  main := |*
    number           => {
      char* dupd = strdup(ts);
      char* substr = string_trimend(dupd, te-ts);
      uint32_t val = strtol(substr, NULL, 10);
      free(substr);
      free(dupd);
      TOKEN(NUMBER);
      cur_token->number_value = val;
    };
    '='              => { TOKEN(ASSIGN); };
    ';'              => { TOKEN(SEMICOLON); };
    "=="             => { TOKEN(EQ); };
    "!="             => { TOKEN(NE); };
    '<'              => { TOKEN(LT); };
    '>'              => { TOKEN(GT); };
    ">="             => { TOKEN(GE); };
    "<="             => { TOKEN(LE); };
    '+'              => { TOKEN(PLUS); };
    '-'              => { TOKEN(MINUS); };
    '*'              => { TOKEN(MULT); };
    '/'              => { TOKEN(DIVIDE); };
    "if"             => { TOKEN(IF); };
    "then"           => { TOKEN(THEN); };
    "else"           => { TOKEN(ELSE); };
    "fi"             => { TOKEN(FI); };
    "do"             => { TOKEN(DO); };
    "od"             => { TOKEN(OD); };
    "while"          => { TOKEN(WHILE); };
    '('              => { TOKEN(LPAREN); };
    ')'              => { TOKEN(RPAREN); };
    alpha_u alpha_u* => {
      char* dupd = strdup(ts);
      char* substr = string_trimend(dupd, te-ts);
      free(dupd);
      TOKEN(NAME);
      cur_token->string_value = substr;
    };
    space;
  *|;

}%%

%% write data nofinal;

#define TOKEN(t) \
  token_t* tok = mk_token(t); \
  if (cur_token != NULL) { \
    cur_token->next = tok; \
    cur_token = cur_token->next; \
  } else { \
    tokens = tok; \
    cur_token = tok; \
  }

token_t* mk_token(type) {
  token_t* tok = malloc(sizeof(token_t));
  tok->type = type;
  tok->next = NULL;
  return tok;
}

token_t* lex(char* bfr) {
  // int curline = 1;
  const char *p = bfr;
  const char *pe = p + strlen(p) + 1;
  const char *eof = pe;
  int cs = 0, act = 0;
  const char *ts, *te;
  %%write init;

  token_t *tokens = NULL;
  token_t *cur_token = tokens;

  %%write exec;

  return tokens;
}

void *ParseAlloc(void *);
void Parse(void *, int, void*, void*);
void ParseFree(void *, void *);

char* read_file(char* path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    printf("failed to read file\n");
    abort();
  }

  fseek(f, 0, SEEK_END);
  int length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = malloc(length);
  if (!buffer) {
    printf("failed to allocate space for the file");
    abort();
  }
  fread(buffer, 1, length, f);
  fclose(f);

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
  printf("%i, %i\n", mks_lexer_error, mks_lexer_en_main);

  char* contents = read_file(argv[1]);
  printf("read file: %s\n", contents);
  token_t *tokens = lex(contents);
  token_t *cur_token = tokens;
  print_tokens(tokens);

  void* parser = ParseAlloc((void*)malloc);

  do {
    Parse(parser, cur_token->type, cur_token, NULL);
  } while ((cur_token = cur_token->next) != NULL);

  Parse(parser, 0, NULL, NULL);

  ParseFree(parser, (void*)free);

  return 0;
}
