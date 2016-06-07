#include <string.h>
#include <stdio.h>
#include "parser.h"

%%{
  machine mks_lexer;

  number = ('+'|'-')?[0-9]+;
  alpha_u = alpha | '_';

  main := |*
    number           => { TOKEN(NUMBER); };
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
    alpha_u alpha_u* => { TOKEN(NAME); };
    space;
  *|;

}%%

%% write data nofinal;

#define TOKEN(t) token->type = t

typedef struct { int type; } token_t;

typedef struct {
  const char *p, *pe, *eof;
  const char *ts, *te;
  int act, cs;
} lexer_t;

lexer_t* lexer_init(char* bfr) {
  int cs = 0, ts = 0, te = 0, act = 0;
  %%write init;

  lexer_t* lexer = malloc(sizeof(lexer_t));
  lexer->p = bfr;
  lexer->pe = lexer->p + strlen(lexer->p) + 1;
  lexer->eof = lexer->pe;

  return lexer;
}

token_t* lexer_next(lexer_t *lexer) {
  const char* p = NULL, * pe = NULL, * eof = NULL;
  const char* ts, * te;
  int act = 0, cs = 0;

  token_t *token = malloc(sizeof(token_t));

  %%write exec;

  return token;
}

int main() {
  printf("%i, %i", mks_lexer_error, mks_lexer_en_main);

  lexer_t *lexer = lexer_init("Hello World");
  lexer_next(lexer);
}
