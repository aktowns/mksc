#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include <stdint.h>
#include <string_utils.h>
#include <token.h>

%%{
  machine mks_lexer;

  newline = '\n' @{ ++line_counter; line_position = (intptr_t)p; };
	c_comment := any* :>> '*)' @{ fgoto main; };

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
    'if'             => { TOKEN(IF); };
    "then"           => { TOKEN(THEN); };
    "else"           => { TOKEN(ELSE); };
    "fi"             => { TOKEN(FI); };
    "do"             => { TOKEN(DO); };
    "od"             => { TOKEN(OD); };
    '('              => { TOKEN(LPAREN); };
    ')'              => { TOKEN(RPAREN); };
    alpha_u alpha_u* => {
      char* dupd = strdup(ts);
      char* substr = string_trimend(dupd, te-ts);
      free(dupd);
      TOKEN(IDENTIFIER);
      cur_token->string_value = substr;
    };

    # Comments and whitespace.
	  '(*' { fgoto c_comment; };
	  '--' [^\n]* '\n';
    ( any - 33..126 )+;
    newline;
  *|;

}%%

#pragma clang diagnostic push
#if defined(__has_warning) && __has_warning("-Wunused-const-variable")
#pragma clang diagnostic ignored "-Wunused-const-variable"
#endif
%% write data nofinal;
#pragma clang diagnostic pop

token_t* mk_token(int type, int line, int col) {
  token_t* tok = malloc(sizeof(token_t));
  tok->type = type;
  tok->line_no = line;
  tok->column_no = col;
  tok->next = NULL;

  return tok;
}

#define TOKEN(t) \
  token_t* tok = mk_token(t, line_counter, (intptr_t)(ts - line_position) + 1); \
  if (cur_token != NULL) { \
    cur_token->next = tok; \
    cur_token = cur_token->next; \
  } else { \
    tokens = tok; \
    cur_token = tok; \
  }

token_t* lex(char* bfr) {
  int line_counter = 1;
  intptr_t line_position = (intptr_t)bfr;
  int len = strlen(bfr);
  char *p = bfr;
  char *pe = p + len;
  char *eof = pe;
  int cs = 0, act = 0;
  char *ts, *te;
  %%write init;

  token_t *tokens = NULL;
  token_t *cur_token = tokens;

  %%write exec;

  return tokens;
}

void token_free(token_t *token) {
  switch(token->type) {
    case IDENTIFIER:
      free(token->string_value);
      break;
  }
  free(token);
}

void lex_free(token_t* tokens) {
  token_t* curr;
  while ((curr = tokens) != NULL) {
    tokens = tokens->next;
    token_free(curr);
  }
}
