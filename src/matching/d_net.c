#include "d_net.h"
#include "vector.h"
#include <string.h>
#include <stdio.h>

struct d_net {
    char* symbol;
	enum matchtype m_type;
    vector* subnets;
    int matchId; //For debugging only
};


d_net* find_subnet(d_net* dn, char* symbol);

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->subnets = vector_init();
    dn->symbol = "";

    return dn;
}

int nextid = 1; //TEMP for debug

void add_pattern(d_net* dn, flatterm* ft) {
    term* t = flatterm_first(ft);

    d_net* subnet = NULL;
    while (t) {
        if ((subnet = find_subnet(dn, t->symbol)) != NULL) {
            dn = subnet;
        } else {
            subnet = net_init();
            subnet->symbol = t->symbol;
            subnet->m_type = t->m_type;
            vector_push_back(dn->subnets, subnet);
            dn = subnet;
        }

        t = t->next;
    }

    subnet->matchId = nextid++;
}

d_net* find_subnet(d_net* dn, char* symbol) {
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        d_net* sn = (d_net*)vector_at(dn->subnets, i);

        if (sn->m_type == MT_CONSTANT) {
            if (strcmp(sn->symbol, symbol) == 0) {
                return sn;
            }
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
        printf(", matchid: %d\n", dn->matchId);
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

list* _match(d_net* dn, flatterm* subject, term* t, vector* subst_vector) {
    if (t == NULL) {
        d_net* subnet = NULL;
        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {
                subst* newSubst = malloc(sizeof(subst));
                newSubst->from = subnet->symbol;
                newSubst->to = "#";
                vector_push_back(subst_vector, newSubst);

                _match(subnet, subject, t, subst_vector);
                vector_pop_back(subst_vector, free);
            }
        }

        if (dn->matchId != 0) {
            printf("Match, id: %d, ", dn->matchId);
            for (int i = 0; i < vector_size(subst_vector); i++) {
                subst* s = vector_at(subst_vector, i);
                printf(" (%s -> %s)", s->from, s->to);
            }
            printf("\n");
        }
    } else {
        d_net* subnet = NULL;
        
        // Symbol matching
        if ((subnet = find_subnet(dn, t->symbol)) != NULL) {
            _match(subnet, subject, t->next, subst_vector);
        }

        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                continue;
            }
            
            // Variable substitution matching, replace whole term.
            subst* newSubst = malloc(sizeof(subst));
            newSubst->from = subnet->symbol;
            newSubst->to = t->symbol;
            vector_push_back(subst_vector, newSubst);

            _match(subnet, subject, t->end->next, subst_vector);
            vector_pop_back(subst_vector, free);

            if (subnet->m_type == MT_STAR) {
                subst* newSubst = malloc(sizeof(subst));
                newSubst->from = subnet->symbol;
                newSubst->to = "#";
                vector_push_back(subst_vector, newSubst);

                _match(subnet, subject, t, subst_vector);
                vector_pop_back(subst_vector, free);
            }

            if (t->parent != NULL) { //Has a parent
                term* vp = t->parent;
                if (subnet->m_type == MT_SEQUENCE || subnet->m_type == MT_STAR) {
                    //j = sista argumentet.
                    term* tEnd = t->next;
                    char strBuffer[1024] = ""; //TODO lazy.
                    strcat(strBuffer, t->symbol);
                    while (tEnd != vp->end->next) {
                        strcat(strBuffer, tEnd->symbol);
                        // t = slå ihop allt mellan nuvarande term och i
                        //matcha med den substitutionen
                        subst* newSubst = malloc(sizeof(subst));
                        newSubst->from = subnet->symbol;
                        newSubst->to = strBuffer;
                        vector_push_back(subst_vector, newSubst);

                        _match(subnet, subject, tEnd->end->next, subst_vector);
                        vector_pop_back(subst_vector, free);
                        tEnd = tEnd->end->next;
                    }
                }
            }
        }
    }
    
    return NULL;
}

list* pattern_match(d_net* dn, flatterm* subject) {
    return _match(dn, subject, flatterm_first(subject), vector_init());
}
