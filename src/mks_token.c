#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "mks_token.h"
#include "utils/ansi_colour.h"
#include "utils/file_utils.h"

void mks_token_iterate(token_t* token_top, token_iterate_cb cb) {
    do {
        cb(token_top);
    } while ((token_top = token_top->next) != NULL);
}

token_t* mk_token(int type, int line, int col) {
    token_t* tok = malloc(sizeof(token_t));
    tok->type = type;
    tok->line_no = line;
    tok->column_no = col;
    tok->next = NULL;

    return tok;
}

void token_free(token_t *token) {
    switch(token->type) {
        case TOKEN_IDENTIFIER:
        case TOKEN_SPECIAL_IDENTIFIER:
        case TOKEN_STRING_LITERAL:
            free(token->string_value);
            break;
        default: break;
    }
    free(token);
}

void tokens_free(token_t* tokens) {
    token_t* curr;
    while ((curr = tokens) != NULL) {
        tokens = tokens->next;
        token_free(curr);
    }
}

const char *string_token(token_t *token) {
    switch (token->type) {
        case TOKEN_MODULE:
            return "MODULE";
        case TOKEN_IMPORT:
            return "IMPORT";
        case TOKEN_IS:
            return "IS";
        case TOKEN_AS:
            return "AS";
        case TOKEN_LET:
            return "LET";
        case TOKEN_NUMBER_LITERAL:
            return "NUMBER";
        case TOKEN_STRING_LITERAL:
            return "STRING";
        case TOKEN_PLUS:
            return "PLUS";
        case TOKEN_SEPERATOR:
            return "SEPERATOR";
        case TOKEN_ASSIGN:
            return "ASSIGN";
        case TOKEN_LPAREN:
            return "LPAREN";
        case TOKEN_RPAREN:
            return "RPAREN";
        case TOKEN_COMMA:
            return "COMMA";
        case TOKEN_IDENTIFIER:
            return "IDENTIFIER";
        case TOKEN_SPECIAL_IDENTIFIER:
            return "SPECIAL_IDENTIFIER";
        case TOKEN_EQ:
            return "EQ";
        case TOKEN_IF:
            return "IF";
        case TOKEN_ELSE:
            return "ELSE";
        case TOKEN_THEN:
            return "THEN";
        case TOKEN_FI:
            return "FI";
        case TOKEN_MINUS:
            return "MINUS";
        case TOKEN_DIVIDE:
            return "DIVIDE";
        case TOKEN_MULT:
            return "MULT";
        case TOKEN_NE:
            return "NE";
        case TOKEN_LT:
            return "LT";
        case TOKEN_LE:
            return "LE";
        case TOKEN_GT:
            return "GT";
        case TOKEN_GE:
            return "GE";
        case TOKEN_DO:
            return "DO";
        case TOKEN_OD:
            return "OD";
        default:
            printf("Unknown token type: %i\n", token->type);
            return "unknown";
    }
}

void print_tokens(token_t *tokens) {
    printf("[ ");
    mks_token_iterate(tokens, ^(token_t* token) {
        printf("%s(%i:%i) ", string_token(token), token->line_no, token->column_no);
    });
    printf("]\n");
}

void annotate_source(char *path, token_t *tokens) {
    printf("%s== Annotating: %s ==%s\n", ANSI_RED, path, ANSI_NORMAL);

    iterate_file(path, ^(char *line, int lineno) {
        printf("%s%i%s: %s", ANSI_GREEN, lineno, ANSI_NORMAL, line);
        printf("%s%i%s: ", ANSI_BLUE, lineno, ANSI_NORMAL);
        mks_token_iterate(tokens, ^(token_t* token) {
            if (token->line_no == lineno) {
                printf("%s ", string_token(token));
            }
        });
        printf("\n");
    });

    printf("%s============%s\n", ANSI_RED, ANSI_NORMAL);
}
