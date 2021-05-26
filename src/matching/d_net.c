#include "d_net.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hash_table_linked.h"

typedef struct net_branch {
    char* symbol;
    int id;
	enum matchtype m_type;
	enum functype f_type;
    vector* subnets;
    int matchId; //For debugging only
} net_branch;

struct d_net {
    net_branch* root;
    hash_table* patternHt;
    hash_table* constantHt;
    vector* idLookup;
    int nextId;
};

net_branch* find_subnet(net_branch* dn, char* symbol, enum functype f_type);

net_branch* branch_init() {
    net_branch* b = malloc(sizeof(net_branch));
    b->subnets = vector_init();
    b->symbol = "";
    b->matchId = 0;
    b->id = 0;

    return b;
}

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->root = branch_init();
    dn->patternHt = create_hash_table(500);
    dn->constantHt = create_hash_table(500);
    dn->idLookup = vector_init();
    vector_push_back(dn->idLookup, NULL);
    dn->nextId = 1;
}



int nextid = 1; //TEMP for debug
int patternsAdded = 0;

void _add_pattern(d_net* dn, net_branch* b, flatterm* ft) {
    term* t = flatterm_first(ft);

    net_branch* subnet = NULL;
    while (t) {
        if ((subnet = find_subnet(b, t->symbol, t->f_type)) != NULL) {
            free(t->symbol);
            b = subnet;
        } else {
            int id = insert_if_absent(dn->patternHt, t->symbol, &dn->nextId);

            if (t->m_type == MT_CONSTANT) {
                insert_entry_unsafe(dn->constantHt, t->symbol, id);
            }

            if (id == vector_size(dn->idLookup)) {
                vector_push_back(dn->idLookup, t->symbol);
            } else if (id > vector_size(dn->idLookup)) { //Remove me later
                fprintf(stderr, "ERROR: id > vector_size(dn->idLookup)\n");
                exit(1);
            }

            subnet = branch_init();
            subnet->symbol = t->symbol;
            subnet->m_type = t->m_type;
            subnet->f_type = t->f_type;
            b->id = id;
            vector_push_back(b->subnets, subnet);
            b = subnet;
        }

        t = t->next;
    }
    patternsAdded += 1;
    subnet->matchId = nextid++;
    flatterm_free(ft);
}

void add_pattern(d_net* dn, flatterm* ft) {
    _add_pattern(dn, dn->root, ft);
}

int get_patterns_added() {
    return patternsAdded;
}

net_branch* find_subnet(net_branch* dn, char* symbol, enum functype f_type) {
    void** sn_data = vector_data(dn->subnets);
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        net_branch* sn = (net_branch*)sn_data[i];

        if (sn->m_type == MT_CONSTANT && f_type == sn->f_type) { 
            if (strcmp(sn->symbol, symbol) == 0) {
                return sn;
            }
        }
    }

    return NULL;
}


int _print_net(net_branch* dn, int offset) {    
    int newBranch = 0;
    printf("%s ", dn->symbol);
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        if (newBranch) {
            for (int i = 0; i <= offset; i++) {
                printf(" ");
            }
        }
        net_branch* sn = (net_branch*)vector_at(dn->subnets, i);
        newBranch = _print_net(sn, offset + strlen(sn->symbol) + 1);
    }
    if (vector_size(dn->subnets) == 0) {
        printf(", matchid: %d\n", dn->matchId);
        return 1;
    }
    return newBranch;
}

void print_net(d_net* dn) {
    net_branch* b = dn->root;
    for (int i = 0; i < vector_size(b->subnets); i++) {
        net_branch* sn = (net_branch*)vector_at(b->subnets, i);
        _print_net(sn, strlen(sn->symbol));
        printf("\n");
    }
}

int strCmps = 0;
/**
 * return int 0 for invalid, 1 for existing, 2 for new
 */
int is_valid_match(char* patternSymbol, subjectFlatterm* sf, int sfLen, s_vector* subst_vector) {
    int foundValid = 2;

    for (size_t i = 0; i < s_vector_size(subst_vector); i++) {
        s_entry* entry = s_vector_at(subst_vector, i);
        
        strCmps += 1;
        if (strcmp(patternSymbol, entry->from) == 0) {
            //fprintf(stderr, "found match on patternSymbol: %s vs entry->from: %s\n", patternSymbol, entry->from);
            //fprintf(stderr, "sf for %s has len %d vs entry->sf for %s has len %d\n", patternSymbol,sfLen, entry->from, entry->len);
            foundValid = 0;

            if (entry->len == sfLen) {
                subjectFlatterm* entrySf = entry->to;
                bool invalid = false;

                for (size_t i = 0; i < sfLen; i++) {
                    
                    if (sf == NULL || entrySf == NULL) {
                        //fprintf(stderr, "sf: %p and entrySf: %p\n", sf, entrySf);
                        invalid = true;
                        break;
                    } else {

                        strCmps += 1;
                        if (strcmp(entrySf->symbol, sf->symbol) != 0) {
                            //fprintf(stderr, "subjectSymbol not same, entrySf: %s vs sf: %s\n", entrySf->symbol, sf->symbol);
                            invalid = true;
                        }
                    }
                    sf = sf->next;
                    entrySf = entrySf->next;
                }
                
                if (invalid == false) {
                    foundValid = 1;
                }
            }
            break;
        }
    }
    return foundValid;
}

int get_strCmps() {
    return strCmps;
}


void _match(net_branch* dn, subjectFlatterm* t, s_vector* subst_vector, vector* matches) {
//void _match(net_branch* dn, flatterm* subject, term* t, vector* subst_vector, vector* matches) {
    if (t == NULL) {
        net_branch* subnet = NULL;
        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (net_branch*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->symbol, NULL, 0, subst_vector)) {
                case 1:
                    _match(subnet, t, subst_vector, matches);
                    break;
                case 2:
                    s_vector_push_back(subst_vector, subnet->symbol, NULL, 0);

                    _match(subnet, t, subst_vector, matches);
                    s_vector_pop_back(subst_vector);
                    break;
                case 0: //invalid match;
                    break;
                }
            }
        }

        if (dn->matchId != 0) {
            net_match* new_match = malloc(sizeof(net_match));
            new_match->matchid = dn->matchId;
            new_match->substitutions = s_vector_copy(subst_vector, &(new_match->subst_amount));
            vector_push_back(matches, new_match);
        }
    } else {
        net_branch* subnet = NULL;
        enum functype f_type = t->f_type;// == t->skip ? FT_NOTAFUNC : FT_PREFIX; //!Need to fix this so this knows if it's a func or not

        void** subnet_data = vector_data(dn->subnets);
        for (int i = 0; i < vector_size(dn->subnets); i++) {
            subnet = (net_branch*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                if (f_type == subnet->f_type) { 
                    if (strcmp(subnet->symbol, t->symbol) == 0) {
                        _match(subnet, t->next, subst_vector, matches);
                    }
                }
                continue;
            }

            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->symbol, NULL, 0, subst_vector)) {
                case 1:
                    _match(subnet, t, subst_vector, matches);
                    break;
                case 2:
                    s_vector_push_back(subst_vector, subnet->symbol, NULL, 0);

                    _match(subnet, t, subst_vector, matches);
                    s_vector_pop_back(subst_vector);
                    break;
                case 0: //invalid match;
                    break;
                }
            }

            switch (is_valid_match(subnet->symbol, t, 1, subst_vector)) {
            case 1:
                if (f_type == FT_PREFIX && subnet->m_type == MT_VARIABLE && subnet->f_type == FT_PREFIX) {
                    // Matching generic function name
                    _match(subnet, t->next, subst_vector, matches);
                } else {
                    // Variable substitution matching, replace whole term.
                    _match(subnet, t->skip, subst_vector, matches);
                }
                break;
            case 2:
                s_vector_push_back(subst_vector, subnet->symbol, t, 1);

                if (f_type == FT_PREFIX && subnet->m_type == MT_VARIABLE && subnet->f_type == FT_PREFIX) {
                    // Matching generic function name
                    _match(subnet, t->next, subst_vector, matches);
                } else {
                    // Variable substitution matching, replace whole term.
                    _match(subnet, t->skip, subst_vector, matches);
                }

                s_vector_pop_back(subst_vector);
                break;
            case 0: //invalid match;
                break;
            }
            
            if (t->parent != NULL) { //Has a parent
                if (subnet->m_type == MT_SEQUENCE || subnet->m_type == MT_STAR) {
                    //j = sista argumentet.
                    subjectFlatterm* vp = t->parent;
                    subjectFlatterm* tEnd = t->skip;
                    int len = 2;
                    while (tEnd != vp->skip) {
                        // t = slå ihop allt mellan nuvarande term och i
                        //matcha med den substitutionen
                        switch (is_valid_match(subnet->symbol, t, len, subst_vector)) {
                        case 1:
                            _match(subnet, tEnd->skip, subst_vector, matches);
                            break;
                        case 2:
                            s_vector_push_back(subst_vector, subnet->symbol, t, len);

                            _match(subnet, tEnd->skip, subst_vector, matches);
                            s_vector_pop_back(subst_vector);
                            break;
                        case 0: //invalid match;
                            break;
                        }
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
    _match(dn->root, subject, substs, matches);
    s_vector_free(substs);
    return matches;
}


void _branch_free(void* netVoid) {
    net_branch* net = (net_branch*)netVoid;
    vector_free(net->subnets, _branch_free);
    free(net->symbol);
    free(net);
}

void net_free(d_net* net) {
    vector_free(net->root->subnets, _branch_free);
    free(net->root);
    delete_hash_table(net->patternHt);
    delete_hash_table(net->constantHt);
    vector_free(net->idLookup, NULL);
    free(net);
    
}