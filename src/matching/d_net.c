#include "d_net.h"
#include <string.h>
#include <stdio.h>

struct d_net {
    char* symbol;
	enum matchtype m_type;
	enum functype f_type;
    vector* subnets;
    int matchId; //For debugging only
};


d_net* find_subnet(d_net* dn, char* symbol, enum functype f_type);

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->subnets = vector_init();
    dn->symbol = "";
    dn->matchId = 0;

    return dn;
}

int nextid = 1; //TEMP for debug
int patternsAdded = 0;

void add_pattern(d_net* dn, flatterm* ft) {
    term* t = flatterm_first(ft);

    d_net* subnet = NULL;
    while (t) {
        if ((subnet = find_subnet(dn, t->symbol, t->f_type)) != NULL) {
            free(t->symbol);
            dn = subnet;
        } else {
            subnet = net_init();
            subnet->symbol = t->symbol;
            subnet->m_type = t->m_type;
            subnet->f_type = t->f_type;
            vector_push_back(dn->subnets, subnet);
            dn = subnet;
        }

        t = t->next;
    }
    patternsAdded += 1;
    subnet->matchId = nextid++;
    flatterm_free(ft);
}

int get_patterns_added() {
    return patternsAdded;
}

d_net* find_subnet(d_net* dn, char* symbol, enum functype f_type) {
    void** sn_data = vector_data(dn->subnets);
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        d_net* sn = (d_net*)sn_data[i];

        if (sn->m_type == MT_CONSTANT && f_type == sn->f_type) { 
            if (strcmp(sn->symbol, symbol) == 0) {
                return sn;
            }
        }
    }

    return NULL;
}


int _print_net(d_net* dn, int offset) {    
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


void _match(d_net* dn, subjectFlatterm* t, s_vector* subst_vector, vector* matches) {
//void _match(d_net* dn, flatterm* subject, term* t, vector* subst_vector, vector* matches) {
    if (t == NULL) {
        d_net* subnet = NULL;
        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {
                s_vector_push_back(subst_vector, subnet->symbol, NULL, 0);

                _match(subnet, t, subst_vector, matches);
                s_vector_pop_back(subst_vector);
            }
        }

        if (dn->matchId != 0) {
            net_match* new_match = malloc(sizeof(net_match));
            new_match->matchid = dn->matchId;
            new_match->substitutions = s_vector_copy(subst_vector, &(new_match->subst_amount));
            vector_push_back(matches, new_match);
        }
    } else {
        d_net* subnet = NULL;
        enum functype f_type = t->f_type;// == t->skip ? FT_NOTAFUNC : FT_PREFIX; //!Need to fix this so this knows if it's a func or not
        
        // Symbol matching
        if ((subnet = find_subnet(dn, t->symbol, f_type)) != NULL) {
            _match(subnet, t->next, subst_vector, matches);
        }

        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                continue;
            }

            if (subnet->m_type == MT_STAR) {
                s_vector_push_back(subst_vector, subnet->symbol, NULL, 0);

                _match(subnet, t, subst_vector, matches);
                s_vector_pop_back(subst_vector);
            }

            s_vector_push_back(subst_vector, subnet->symbol, t, 1);

            if (f_type == FT_PREFIX && subnet->m_type == MT_VARIABLE && subnet->f_type == FT_PREFIX) {
                // Matching generic function name
                _match(subnet, t->next, subst_vector, matches);
            } else {
                // Variable substitution matching, replace whole term.
                _match(subnet, t->skip, subst_vector, matches);
            }

            s_vector_pop_back(subst_vector);

            if (t->parent != NULL) { //Has a parent
                if (subnet->m_type == MT_SEQUENCE || subnet->m_type == MT_STAR) {
                    //j = sista argumentet.
                    subjectFlatterm* vp = t->parent;
                    subjectFlatterm* tEnd = t->skip;
                    int len = 2;
                    while (tEnd != vp->skip) {
                        // t = slå ihop allt mellan nuvarande term och i
                        //matcha med den substitutionen
                        s_vector_push_back(subst_vector, subnet->symbol, t, len);

                        _match(subnet, tEnd->skip, subst_vector, matches);
                        s_vector_pop_back(subst_vector);
                        tEnd = tEnd->skip;
                        len++;

                    }
                }
            }
        }
    }
}


vector* pattern_match(d_net* dn, subjectFlatterm* subject) {
    vector* matches = vector_init();
    s_vector* substs = s_vector_init();
    _match(dn, subject, substs, matches);
    s_vector_free(substs);
    return matches;
}


void _branch_free(void* netVoid) {
    d_net* net = (d_net*)netVoid;
    vector_free(net->subnets, _branch_free);
    free(net->symbol);
    free(net);
}

void net_free(d_net* net) {
    vector_free(net->subnets, _branch_free);
    free(net);
}