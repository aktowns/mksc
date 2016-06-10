#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <stdlib.h>

char *string_substr(char *src, size_t start, size_t end);

char *string_trimstart(char *src, size_t start);

char *string_trimend(char *src, size_t end);

#endif
