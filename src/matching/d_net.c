#include "d_net.h"
#include "list.h"
#include "vector.h"
#include <string.h>
#include <stdio.h>

struct d_net {
    char* symbol;
    vector* subnets;
};


d_net* find_subnet(d_net* dn, char* symbol);

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->subnets = vector_init();
    dn->symbol = "";

    return dn;
}

void add_pattern(d_net* dn, list* flatterm) {
    if (list_isEmpty(flatterm)) {
        return;
    }

    list_pos ft_pos = list_first(flatterm);

    while (1) {
        term* t = list_inspect(flatterm, ft_pos);
        d_net* subnet = NULL;
        if ((subnet = find_subnet(dn, t->symbol)) != NULL) {
            dn = subnet;
        } else {
            subnet = net_init();
            subnet->symbol = t->symbol;
            vector_push_back(dn->subnets, subnet);
            dn = subnet;
        }


        if (list_isEnd(flatterm, ft_pos)) {
            break;
        }
        ft_pos = list_next(flatterm, ft_pos);
    }
}

d_net* find_subnet(d_net* dn, char* symbol) {
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        d_net* sn = (d_net*)vector_at(dn->subnets, i);

        if (strcmp(sn->symbol, symbol) == 0) {
            return sn;
        }
    }

    return NULL;
}


int _print_net(d_net* dn, int offset) {
    // printf("%s, children:", dn->symbol);

    // for (int i = 0; i < vector_size(dn->subnets); i++) {
    //     printf(" %s", ((d_net*)(vector_at(dn->subnets, i)))->symbol);
    // }
    // printf("\n");

    
    int newBranch = 0;
    printf("%s ", dn->symbol);
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        if (newBranch) {
            for (int i = 0; i <= offset; i++) {
                printf(" ");
            }
        }
        d_net* sn = (d_net*)vector_at(dn->subnets, i);
        newBranch = _print_net(sn, offset + strlen(sn->symbol) + 1);
    }
    if (vector_size(dn->subnets) == 0) {
        printf("\n");
        return 1;
    }
    return newBranch;
}

void print_net(d_net* dn) {
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        d_net* sn = (d_net*)vector_at(dn->subnets, i);
        _print_net(sn, strlen(sn->symbol));
        printf("\n");
    }
}
