#ifndef MC_PARSER
#define MC_PARSER

#include "flatterm.h"
#include "hash_table_linked.h"

flatterm* parsePattern(const char str[], hash_table* ht_constants);
void debugPattern(const char str[]);

#endif
