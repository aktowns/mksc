%include
{
    #include <stdio.h>
    #include <assert.h>
}

%syntax_error
{
    printf("Error parsing command\n");
}

%token_type { const char* }

start ::= stmtseq.

statement ::= designator ASSIGN expression.
statement ::= PRINT expression.
statement ::= IF expression THEN stmtseq ELSE stmtseq FI.
statement ::= IF expression THEN stmtseq FI.
statement ::= WHILE expression DO stmtseq OD .

stmtseq ::= stmtseq SEMICOLON statement.
stmtseq ::= statement.

expression ::= expr2.
expression ::= expr2 EQ expr2.
expression ::= expr2 NE expr2.
expression ::= expr2 LT expr2.
expression ::= expr2 LE expr2.
expression ::= expr2 GT expr2.
expression ::= expr2 GE expr2.

expr2 ::= expr3.
expr2 ::= expr2 PLUS expr3.
expr2 ::= expr2 MINUS expr3.

expr3 ::= expr4.
expr3 ::= expr3 MULT expr4.
expr3 ::= expr3 DIVIDE expr4.

expr4 ::= PLUS expr4.
expr4 ::= MINUS expr4.
expr4 ::= LPAREN expression RPAREN.
expr4 ::= NUMBER.
expr4 ::= designator.

designator ::= NAME.
