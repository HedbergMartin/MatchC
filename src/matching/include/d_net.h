#ifndef MC_DNET
#define MC_DNET

#include "flatterm.h"
#include "subjectFlatterm.h"
#include "vector.h"
#include "s_vector.h"

typedef struct d_net d_net;

typedef struct net_match {
    int matchid;
    s_entry* substitutions;
    int subst_amount;
} net_match;

d_net* net_init();
void add_pattern(d_net* dn, flatterm* ft);
void print_net(d_net* dn);
vector* pattern_match(d_net* dn, subjectFlatterm* subject);

#endif