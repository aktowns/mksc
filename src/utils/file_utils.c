#include <stdio.h>
#include <stdlib.h>
#include "utils/file_utils.h"

char *read_file(char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        printf("failed to read file\n");
        abort();
    }

    fseek(f, 0, SEEK_END);
    size_t length = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buffer = malloc(length + 1);
    if (!buffer) {
        printf("failed to allocate space for the file");
        abort();
    }
    fread(buffer, 1, length, f);
    fclose(f);

    buffer[length] = '\0';

    return buffer;
}

void iterate_file(char *path, iterate_file_cb cb) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;

    fp = fopen(path, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int lineno = 1;
    while (getline(&line, &len, fp) != -1) {
        cb(line, lineno);
        lineno += 1;
    }

    fclose(fp);
    if (line) {
        free(line);
    }
}