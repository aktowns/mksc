#ifndef __PARSER_EXPORTS_H
#define __PARSER_EXPORTS_H

void *ParseAlloc(void *);

void Parse(void *, int, void *, void *);

void ParseFree(void *, void *);

#endif