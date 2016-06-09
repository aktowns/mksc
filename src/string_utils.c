#include <stdlib.h>
#include <string.h>

char *string_substr(char *src, int start, int end) {
  if (src == NULL) abort();
  if (strlen(src) < (end-start)) abort();
  int str_len = end-start;

  char *new_str = malloc(str_len + 1);
  strncpy(new_str, &src[start], str_len);
  new_str[str_len] = '\0';

  return new_str;
}

char *string_trimstart(char *src, int start) {
  int end = strlen(src)-start;
  return string_substr(src, start, end);
}

char *string_trimend(char *src, int end) {
  return string_substr(src, 0, end);
}

