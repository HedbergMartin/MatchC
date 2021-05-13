#ifndef MC_DNET
#define MC_DNET

#include "flatterm.h"

typedef struct d_net d_net;

d_net* net_init();
void add_pattern(d_net* dn, list* flatterm);
void print_net(d_net* dn);

#endif