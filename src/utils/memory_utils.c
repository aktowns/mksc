#include <stdio.h>
#include <stdlib.h>

#include "utils/memory_utils.h"
#include "utils/log_utils.h"

void memory_alloc(size_t sz, memory_alloc_cb cb) {
    void *mem = malloc(sz);

    ASSERT(mem != NULL, "Failed to allocate memory");

    cb(mem);
    free(mem);
}
