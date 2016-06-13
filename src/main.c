#include "mks_node.h"
#include "mks_token.h"
#include "mks_enrichment.h"
#include "parser_exports.h"
#include "utils/ansi_colour.h"
#include "utils/file_utils.h"

#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // emit_module();
    if ((argc < 2) || access(argv[1], F_OK) == -1) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(-1);
    }

    printf("mksc compiler 0.01 ashley towns <mail@ashleytowns.id.au>\n");
    char *contents = read_file(argv[1]);

    token_t *tokens = lex(contents);

    //print_tokens(tokens);
    annotate_source(argv[1], tokens);

    void *parser = ParseAlloc((void *) malloc);

    // ParseTrace(stdout, "* > ");

    mks_token_iterate(tokens, ^(token_t *token) { Parse(parser, token->type, token, NULL); });

    mks_node_t *ast = NULL;

    Parse(parser, 0, NULL, &ast);

    if (ast != NULL && ast->is_ok) {
        initialize_scoping(ast, NULL);

        printf("%sAST%s: ", RGB(100, 200, 100), ANSI_NORMAL);
        pretty_print_node(ast);
        printf("\n");

        printf("%sType enrichment%s: ", RGB(100, 100, 200), ANSI_NORMAL);
        enrich_tree(ast);
        pretty_print_node(ast);

        printf("\n");
        report_exports(ast);
    }

    mks_free_node(ast);
    tokens_free(tokens);
    ParseFree(parser, (void *) free);

    free(contents);

    return 0;
}
