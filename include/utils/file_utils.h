#ifndef __FILE_UTILS_H
#define __FILE_UTILS_H

typedef void (^iterate_file_cb)(char *, int);

char *read_file(char *path);

void iterate_file(char *path, iterate_file_cb cb);

#endif
