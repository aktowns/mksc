%include
{
    #include <stdio.h>
    #include <assert.h>
    #include <mks_node.h>
}

%syntax_error
{
    printf("Error parsing command\n");
}

%token_type { mks_node_t * }

start ::= stmtseq(A). { pretty_print_node(A); }

statement(A) ::= designator(B) ASSIGN expression(C).
{ A = mk_assignment(B, C); }
statement ::= PRINT expression.
statement(A) ::= IF expression(B) THEN stmtseq(C) ELSE stmtseq(D) FI.
{ A = mk_if_stmt(B, C, D); }
statement(A) ::= IF expression(B) THEN stmtseq(C) FI.
{ A = mk_if_stmt(B, C, mk_empty()); }
statement(A) ::= WHILE expression(B) DO stmtseq(C) OD.
{ A = mk_while_stmt(B, C); }

stmtseq(A) ::= stmtseq(B) SEMICOLON statement(C). { A = mk_sequence(B, C); }
stmtseq(A) ::= statement(B). { A = B; }

expression(A) ::= expr2(B). { A = B; }
expression(A) ::= expr2(B) EQ expr2(C). { A = mk_eq_op(B, C); }
expression(A) ::= expr2(B) NE expr2(C). { A = mk_ne_op(B, C); }
expression(A) ::= expr2(B) LT expr2(C). { A = mk_lt_op(B, C); }
expression(A) ::= expr2(B) LE expr2(C). { A = mk_le_op(B, C); }
expression(A) ::= expr2(B) GT expr2(C). { A = mk_gt_op(B, C); }
expression(A) ::= expr2(B) GE expr2(C). { A = mk_ge_op(B, C); }

expr2(A) ::= expr3(B). { A = B; }
expr2(A) ::= expr2(B) PLUS expr3(C). { A = mk_plus_op(B, C); }
expr2(A) ::= expr2(B) MINUS expr3(C). { A = mk_minus_op(B, C); }

expr3(A) ::= expr4(B). { A = B; }
expr3(A) ::= expr3(B) MULT expr4(C). { A = mk_mult_op(B, C); }
expr3(A) ::= expr3(B) DIVIDE expr4(C). { A = mk_divide_op(B, C); }

expr4(A) ::= PLUS expr4(B). { A = B; }
expr4 ::= MINUS expr4.
expr4(A) ::= LPAREN expression(B) RPAREN. { A = B; }
expr4(A) ::= NUMBER. { A = mk_number(10); }
expr4(A) ::= designator(B). { A = B; }

designator(A) ::= NAME. { A = mk_identifier("Hello World"); }
