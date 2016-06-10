#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "parser.h"
#include "mks_token.h"
#include "utils/string_utils.h"

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
      TOKEN(TOKEN_NUMBER);
      cur_token->number_value = val;
    };
    '='              => { TOKEN(TOKEN_ASSIGN); };
    ';'              => { TOKEN(TOKEN_SEMICOLON); };
    "=="             => { TOKEN(TOKEN_EQ); };
    "!="             => { TOKEN(TOKEN_NE); };
    '<'              => { TOKEN(TOKEN_LT); };
    '>'              => { TOKEN(TOKEN_GT); };
    ">="             => { TOKEN(TOKEN_GE); };
    "<="             => { TOKEN(TOKEN_LE); };
    '+'              => { TOKEN(TOKEN_PLUS); };
    '-'              => { TOKEN(TOKEN_MINUS); };
    '*'              => { TOKEN(TOKEN_MULT); };
    '/'              => { TOKEN(TOKEN_DIVIDE); };
    'if'             => { TOKEN(TOKEN_IF); };
    "then"           => { TOKEN(TOKEN_THEN); };
    "else"           => { TOKEN(TOKEN_ELSE); };
    "fi"             => { TOKEN(TOKEN_FI); };
    "do"             => { TOKEN(TOKEN_DO); };
    "od"             => { TOKEN(TOKEN_OD); };
    '('              => { TOKEN(TOKEN_LPAREN); };
    ')'              => { TOKEN(TOKEN_RPAREN); };
    alpha_u alpha_u* => {
      char* dupd = strdup(ts);
      char* substr = string_trimend(dupd, te-ts);
      free(dupd);
      TOKEN(TOKEN_IDENTIFIER);
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