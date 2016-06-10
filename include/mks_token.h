#ifndef __MKS_TOKEN_H
#define __MKS_TOKEN_H

typedef struct node {
    int type;

    int line_no;
    int column_no;

    union {
        long int number_value;
        char *string_value;
    };
    struct node *next;
} token_t;

typedef void (^token_iterate_cb)(token_t*);

token_t *lex(char *bfr);

token_t* mk_token(int type, int line, int col);

void token_free(token_t *token);

void tokens_free(token_t* tokens);

const char *string_token(token_t *token);

void print_tokens(token_t *tokens);

void annotate_source(char *path, token_t *tokens);

void mks_token_iterate(token_t* token_top, token_iterate_cb cb);
#endif
