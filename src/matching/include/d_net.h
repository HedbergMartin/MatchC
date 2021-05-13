#ifndef MC_DNET
#define MC_DNET

#include "flatterm.h"
#include "list.h"

typedef struct d_net d_net;

typedef struct match {
    char* expr;
    flatterm* substitutions;
} match;

typedef struct subst {
    char* from;
    char* to;
} subst;

d_net* net_init();
void add_pattern(d_net* dn, flatterm* ft);
void print_net(d_net* dn);
list* pattern_match(d_net* dn, flatterm* subject);

#endif