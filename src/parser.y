%include {
    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <stdbool.h>

    #include "mks_node.h"
    #include "mks_token.h"

    #pragma clang diagnostic ignored "-Wunused-variable"

    static bool is_ok = true;
}

%syntax_error {
    printf("syntax error, expecting one of: ");
    int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
    for (int i = 0; i < n; ++i) {
        int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
        if (a < YYNSTATE + YYNRULE) {
            printf("%s ", yyTokenName[i]);
        }
    }
    printf("\n");

    is_ok = false;
}

%token_prefix TOKEN_
%token_type { token_t * }
%default_type { mks_node_t * }
%extra_argument { mks_node_t **result }

%destructor top { mks_free($$); }
%destructor toplevelseq { mks_free($$); }
%destructor statement { mks_free($$); }
%destructor body { mks_free($$); }
%destructor expression { mks_free($$); }
%destructor bodyseq { mks_free($$); }
%destructor literal { mks_free($$); }
%destructor identifier { mks_free($$); }
%destructor special_identifier { mks_free($$); }

%type top { mks_node_t * }
%start_symbol top

%left PLUS MINUS.
%left MULT DIVIDE.

%nonassoc EQ NE GT GE LT LE.

top ::= MODULE special_identifier(B) IS toplevelseq(C) SEPERATOR. {
    mks_node_t* res = mk_module(B, C, mk_node(EMPTY));
    res->is_ok = is_ok;
    *result = res;
}
top ::= MODULE special_identifier(B) LPAREN arglist(D) RPAREN IS toplevelseq(C) SEPERATOR. {
    mks_node_t* res = mk_module(B, C, D);
    res->is_ok = is_ok;
    *result = res;
}

toplevelseq(A) ::= toplevelseq(B) SEPERATOR statement(C). { A = mk_sequence(B, C); }
toplevelseq(A) ::= statement(B). { A = B; }

statement(A) ::= LET identifier(B) ASSIGN expression(C). { A = mk_assignment(B, C); }
statement(A) ::= LET identifier(B) ASSIGN DO bodyseq(C) SEPERATOR OD. { A = mk_assignment(B, C); }
statement(A) ::= LET identifier(B) LPAREN arglist(C) RPAREN ASSIGN expression(D).
{ A = mk_assignment(B, mk_function(C, D)); }
statement(A) ::= LET identifier(B) LPAREN arglist(C) RPAREN ASSIGN DO bodyseq(D) SEPERATOR OD.
{ A = mk_assignment(B, mk_function(C, D)); }

statement(A) ::= IMPORT special_identifier(B). { A = mk_import(B, mk_node(EMPTY), mk_node(EMPTY)); }
statement(A) ::= IMPORT special_identifier(B) LPAREN arglist(C) RPAREN. { A = mk_import(B, mk_node(EMPTY), C); }

statement(A) ::= IMPORT special_identifier(B) AS special_identifier(C). { A = mk_import(B, C, mk_node(EMPTY)); }
statement(A) ::= IMPORT special_identifier(B) LPAREN arglist(D) RPAREN AS special_identifier(C).
{ A = mk_import(B, C, D); }

body(A) ::= statement(B). { A = B; }
body(A) ::= expression(B). { A = B; }

bodyseq(A) ::= bodyseq(B) SEPERATOR body(C). { A = mk_sequence(B, C); }
bodyseq(A) ::= body(B). { A = B; }

// gah im doing this wrong.
expression(A) ::= IF expression(B) THEN expression(C) ELSE expression(D) FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN expression(C) ELSE bodyseq(D) SEPERATOR FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN bodyseq(C) SEPERATOR ELSE bodyseq(D) SEPERATOR FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN bodyseq(C) SEPERATOR ELSE expression(D) FI. { A = mk_if_expr(B, C, D); }

expression(A) ::= expression(B) EQ expression(C). { A = mk_eq_operator(B, C); }
expression(A) ::= expression(B) NE expression(C). { A = mk_ne_operator(B, C); }
expression(A) ::= expression(B) LT expression(C). { A = mk_lt_operator(B, C); }
expression(A) ::= expression(B) LE expression(C). { A = mk_le_operator(B, C); }
expression(A) ::= expression(B) GT expression(C). { A = mk_gt_operator(B, C); }
expression(A) ::= expression(B) GE expression(C). { A = mk_ge_operator(B, C); }

expression(A) ::= expression(B) PLUS expression(C). { A = mk_plus_operator(B, C); }
expression(A) ::= expression(B) MINUS expression(C). { A = mk_minus_operator(B, C); }
expression(A) ::= expression(B) MULT expression(C). { A = mk_mult_operator(B, C); }
expression(A) ::= expression(B) DIVIDE expression(C). { A = mk_divide_operator(B, C); }

expression(A) ::= identifier(B) LPAREN arglist(C) RPAREN. { A = mk_function_call(B, C); }
expression(A) ::= LPAREN expression(B) RPAREN. { A = B; }

expression(A) ::= literal(B). { A = B; }
expression(A) ::= identifier(B). { A = B; }
expression(A) ::= identifier_index(B). { A = B; }

arglist(A) ::= expression(B). { A = B; }
arglist(A) ::= arglist(B) COMMA expression(C). { A = mk_sequence(B, C); }

literal(A) ::= NUMBER_LITERAL(B). { A = mk_number(B->number_value); }
literal(A) ::= STRING_LITERAL(B). { A = mk_string(B->string_value); }

identifier(A) ::= IDENTIFIER(B). { A = mk_identifier(B->string_value); }

identifier_index(A) ::= IDENTIFIER(B) LBRACK expression(C) RBRACK.
{ A = mk_function_call(mk_identifier("array_index"), mk_sequence(mk_identifier(B->string_value), C)); }

special_identifier(A) ::= SPECIAL_IDENTIFIER(B). { A = mk_identifier(B->string_value); }
