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

    return dn;
}

int nextid = 1; //TEMP for debug

void add_pattern(d_net* dn, flatterm* ft) {
    term* t = flatterm_first(ft);

    d_net* subnet = NULL;
    while (t) {
        if ((subnet = find_subnet(dn, t->symbol, t->f_type)) != NULL) {
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

    subnet->matchId = nextid++;
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

void _match(d_net* dn, flatterm* subject, term* t, vector* subst_vector, vector* matches) {
    if (t == NULL) {
        d_net* subnet = NULL;
        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {
                subst newSubst;
                newSubst.from = subnet->symbol;
                newSubst.to = "#";
                vector_push_back(subst_vector, &newSubst);

                _match(subnet, subject, t, subst_vector, matches);
                vector_pop_back(subst_vector, NULL);
            }
        }

        if (dn->matchId != 0) {
            char* matchString = malloc(1024 * sizeof(char)); //TODO improve
            char buffer[30];
            sprintf(buffer, "Match! ID: %d,", dn->matchId);
            strcat(matchString, buffer);
            for (int i = 0; i < vector_size(subst_vector); i++) {
                subst* s = vector_at(subst_vector, i);
                sprintf(buffer, " (%s -> %s)", s->from, s->to);
                strcat(matchString, buffer);
            }

            vector_push_back(matches, matchString);
        }
    } else {
        d_net* subnet = NULL;
        
        // Symbol matching
        if ((subnet = find_subnet(dn, t->symbol, t->f_type)) != NULL) {
            _match(subnet, subject, t->next, subst_vector, matches);
        }

        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (d_net*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                continue;
            }

            if (subnet->f_type != t->f_type) {
                continue;
            }
            
            // Variable substitution matching, replace whole term.
            subst* newSubst = malloc(sizeof(subst));
            newSubst->from = subnet->symbol;
            newSubst->to = t->symbol;
            vector_push_back(subst_vector, newSubst);

            _match(subnet, subject, t->next, subst_vector, matches);
            vector_pop_back(subst_vector, free);

            if (subnet->m_type == MT_STAR) {
                subst newSubst;
                newSubst.from = subnet->symbol;
                newSubst.to = "#";
                vector_push_back(subst_vector, &newSubst);

                _match(subnet, subject, t, subst_vector, matches);
                vector_pop_back(subst_vector, NULL);
            }

            if (t->parent != NULL) { //Has a parent
                if (subnet->m_type == MT_SEQUENCE || subnet->m_type == MT_STAR) {
                    term* vp = t->parent;
                    term* tEnd = t->next;
                    char strBuffer[1024] = ""; //TODO lazy.
                    strcat(strBuffer, t->symbol);
                    while (tEnd != vp->end->next) {
                        strcat(strBuffer, tEnd->symbol);
                        // t = slå ihop allt mellan nuvarande term och i
                        //matcha med den substitutionen
                        subst newSubst;
                        newSubst.from = subnet->symbol;
                        newSubst.to = strBuffer;
                        vector_push_back(subst_vector, &newSubst);

                        _match(subnet, subject, tEnd->end->next, subst_vector, matches);
                        vector_pop_back(subst_vector, NULL);
                        tEnd = tEnd->end->next;
                    }
                }
            }
        }
    }
}

vector* pattern_match(d_net* dn, flatterm* subject) {
    vector* matches = vector_init();
    vector* subst_vector = vector_init();
    _match(dn, subject, flatterm_first(subject), subst_vector, matches);
    vector_free(subst_vector, NULL);
    return matches;
}
