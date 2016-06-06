%{
#include <mks_node.h>

#include "parser.h"
#include "lexer.h"

int yyerror(mks_node_t **expression, yyscan_t scanner, const char *msg) {
    // Add error handling routine as needed
    return 0;
}
%}

%code requires {

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}

%output "./src/gen/parser.c"
%defines "./src/gen/parser.h"

%define api.pure
%lex-param { yyscan_t scanner }
%parse-param { mks_node_t **expression }
%parse-param { yyscan_t scanner }

%union {
  mks_node_t *expression;
}

%token TOKEN_EQ

%type <expression> expr

%%

top
  : expr { *expression = $1; }
  ;

expr
  : TOKEN_EQ { $$ = mk_identifier("eq"); }
  ;

%%

