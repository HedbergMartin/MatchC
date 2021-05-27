#ifndef MC_DNET
#define MC_DNET

#include "flatterm.h"
#include "vector.h"
#include "hash_table_linked.h"
#include "subjectFlatterm.h"
#include "match_entry.h"

typedef struct d_net d_net;

d_net* net_init();
void add_pattern(d_net* dn, flatterm* ft);
void print_net(d_net* dn);
match_set* pattern_match(d_net* dn, char* subject);
void net_free(d_net* net);
hash_table* getSymbolHt(d_net* dn);
int net_nextId(d_net* dn);

match_set* pattern_match_measure(d_net* dn, subjectFlatterm* ft_subject);

#endif