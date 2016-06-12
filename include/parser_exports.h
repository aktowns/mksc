#ifndef __PARSER_EXPORTS_H
#define __PARSER_EXPORTS_H

#include <stdio.h>

void *ParseAlloc(void *);

void Parse(void *, int, void *, mks_node_t **);

void ParseTrace(FILE *, char *);

void ParseFree(void *, void *);

#endif
