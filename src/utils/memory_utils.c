#include <stdio.h>
#include <stdlib.h>

#include "utils/memory_utils.h"

void memory_alloc(size_t sz, memory_alloc_cb cb) {
    void* mem = malloc(sz);
    if (mem == NULL) {
        puts("Failed to allocate memory!");
        abort();
    }
    cb(mem);
    free(mem);
}