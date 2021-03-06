%include {
    #include <stdlib.h>
    #include <stdio.h>
    #include <assert.h>
    #include <stdbool.h>
    #include <string.h>

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

%destructor top { mks_free_node($$); }
%destructor toplevelseq { mks_free_node($$); }
%destructor statement { mks_free_node($$); }
%destructor body { mks_free_node($$); }
%destructor expression { mks_free_node($$); }
%destructor bodyseq { mks_free_node($$); }
%destructor identlist { mks_free_node($$); }
%destructor arglist { mks_free_node($$); }
%destructor literal { mks_free_node($$); }
%destructor identifier { mks_free_node($$); }
%destructor special_identifier { mks_free_node($$); }

%type top { mks_node_t * }
%start_symbol top

%left PLUS MINUS.
%left MULT DIVIDE.

%nonassoc EQ NE GT GE LT LE.

top ::= MODULE special_identifier(B) IS toplevelseq(C) SEPARATOR. {
    mks_node_t* res = mk_module(B, C, mk_node(NODE_EMPTY));
    res->is_ok = is_ok;
    *result = res;
}
top ::= MODULE special_identifier(B) LPAREN arglist(D) RPAREN IS toplevelseq(C) SEPARATOR. {
    mks_node_t* res = mk_module(B, C, D);
    res->is_ok = is_ok;
    *result = res;
}

toplevelseq(A) ::= toplevelseq(B) SEPARATOR statement(C). { A = mk_sequence(B, C); }
toplevelseq(A) ::= statement(B). { A = B; }

statement(A) ::= LET identifier(B) ASSIGN expression(C). { A = mk_assignment(B, C); }
statement(A) ::= LET identifier(B) ASSIGN DO bodyseq(C) SEPARATOR OD. { A = mk_assignment(B, C); }

statement(A) ::= LET identifier(B) LPAREN identlist(C) RPAREN ASSIGN expression(D). {
    A = mk_assignment(B, mk_sequence_to_function(C, D));
}
statement(A) ::= LET identifier(B) LPAREN identlist(C) RPAREN ASSIGN DO bodyseq(D) SEPARATOR OD. {
    A = mk_assignment(B, mk_sequence_to_function(C, D));
}

statement(A) ::= IMPORT special_identifier(B). { A = mk_import(B, mk_node(NODE_EMPTY), mk_node(NODE_EMPTY)); }
statement(A) ::= IMPORT special_identifier(B) LPAREN identlist(C) RPAREN. { A = mk_import(B, mk_node(NODE_EMPTY), C); }

statement(A) ::= IMPORT special_identifier(B) AS special_identifier(C). { A = mk_import(B, C, mk_node(NODE_EMPTY)); }
statement(A) ::= IMPORT special_identifier(B) LPAREN identlist(D) RPAREN AS special_identifier(C).
{ A = mk_import(B, C, D); }

body(A) ::= statement(B). { A = B; }
body(A) ::= expression(B). { A = B; }

bodyseq(A) ::= bodyseq(B) SEPARATOR body(C). { A = mk_sequence(B, C); }
bodyseq(A) ::= body(B). { A = B; }

// gah im doing this wrong.
expression(A) ::= IF expression(B) THEN expression(C) ELSE expression(D) FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN expression(C) ELSE bodyseq(D) SEPARATOR FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN bodyseq(C) SEPARATOR ELSE bodyseq(D) SEPARATOR FI. { A = mk_if_expr(B, C, D); }
expression(A) ::= IF expression(B) THEN bodyseq(C) SEPARATOR ELSE expression(D) FI. { A = mk_if_expr(B, C, D); }

expression(A) ::= expression(B) EQ expression(C). { A = mk_operator(B, C, OP_EQ); }
expression(A) ::= expression(B) NE expression(C). { A = mk_operator(B, C, OP_NE); }
expression(A) ::= expression(B) LT expression(C). { A = mk_operator(B, C, OP_LT); }
expression(A) ::= expression(B) LE expression(C). { A = mk_operator(B, C, OP_LE); }
expression(A) ::= expression(B) GT expression(C). { A = mk_operator(B, C, OP_GT); }
expression(A) ::= expression(B) GE expression(C). { A = mk_operator(B, C, OP_GE); }

expression(A) ::= expression(B) PLUS expression(C). { A = mk_operator(B, C, OP_PLUS); }
expression(A) ::= expression(B) MINUS expression(C). { A = mk_operator(B, C, OP_MINUS); }
expression(A) ::= expression(B) MULT expression(C). { A = mk_operator(B, C, OP_MULT); }
expression(A) ::= expression(B) DIVIDE expression(C). { A = mk_operator(B, C, OP_DIVIDE); }

expression(A) ::= identifier(B) LPAREN arglist(C) RPAREN. { A = mk_function_call(B, C); }
expression(A) ::= LPAREN expression(B) RPAREN. { A = B; }

expression(A) ::= literal(B). { A = B; }
expression(A) ::= identifier(B). { A = B; }
expression(A) ::= identifier_index(B). { A = B; }

identlist(A) ::= identifier(B). { A = B; }
identlist(A) ::= identlist(B) COMMA identifier(C). { A = mk_sequence(B, C); }

arglist(A) ::= expression(B). { A = B; }
arglist(A) ::= arglist(B) COMMA expression(C). { A = mk_sequence(B, C); }

literal(A) ::= NUMBER_LITERAL(B). { A = mk_number(B->number_value); }
literal(A) ::= STRING_LITERAL(B). { A = mk_string(B->string_value); }
literal(A) ::= LBRACK arglist(B) RBRACK. { A = mk_array(B); }
literal(A) ::= LPAREN expression(B) COMMA expression(C) RPAREN. { A = mk_tuple(B, C); }
literal(A) ::= TRUE. { A = mk_boolean(true); }
literal(A) ::= FALSE. { A = mk_boolean(false); }

identifier(A) ::= IDENTIFIER(B). { A = mk_identifier(B->string_value); }

identifier_index(A) ::= IDENTIFIER(B) LBRACK expression(C) RBRACK.
{ A = mk_function_call(mk_identifier(strdup("array_index")), mk_sequence(mk_identifier(B->string_value), C)); }

special_identifier(A) ::= SPECIAL_IDENTIFIER(B). { A = mk_identifier(B->string_value); }
