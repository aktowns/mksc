%{
#include <mks_node.h>
#include "parser.h"

#include <stdio.h>
%}
%option warn nodefault

%option noinput nounput
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

%option outfile="./src/gen/lexer.c" header-file="./src/gen/lexer.h"

%%
"=" { return TOKEN_EQ; }
%%

void yyerror (void *scanner, void* consumer, const char *msg) {
    fprintf(stderr, "%s\n", msg);

    abort();
}
