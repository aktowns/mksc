#ifndef __MEMORY_UTILS_H
#define __MEMORY_UTILS_H

typedef void (^memory_alloc_cb)(void *);

void memory_alloc(size_t sz, memory_alloc_cb cb);

#endif
