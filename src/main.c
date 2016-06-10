#include "mks_node.h"
#include "mks_token.h"
#include "parser_exports.h"
#include "utils/ansi_colour.h"
#include "utils/file_utils.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if ((argc < 2) || access(argv[1], F_OK) == -1) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    char *contents = read_file(argv[1]);

    token_t *tokens = lex(contents);

    print_tokens(tokens);
    annotate_source(argv[1], tokens);

    void *parser = ParseAlloc(malloc);

    mks_token_iterate(tokens, ^(token_t* token) { Parse(parser, token->type, token, NULL); });

    tokens_free(tokens);

    mks_node_t *ast = NULL;
    Parse(parser, 0, NULL, &ast);

    if (ast != NULL) {
        printf("%sAST%s: ", RGB(200, 200, 200), ANSI_NORMAL);
        pretty_print_node(ast);
        mks_free(ast);
    }

    ParseFree(parser, free);

    free(contents);

    return 0;
}
