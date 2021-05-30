#include "d_net.h"
#include "time.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "subjectFlatterm.h"
#include "match_entry.h"
#include "parser.h"

typedef struct net_branch {
    char* symbol;
    int id;
	enum matchtype m_type;
	enum functype f_type;
    vector* subnets;
    int isMatch; //For debugging only
} net_branch;

//!Make sure to find max variable depth
struct d_net {
    net_branch* root;
    hash_table* symbolHt;
    vector* idLookup;
};

net_branch* find_subnet(net_branch* dn, int id, enum matchtype m_type, enum functype f_type);

net_branch* branch_init() {
    net_branch* b = malloc(sizeof(net_branch));
    b->subnets = vector_init();
    b->symbol = "";
    b->isMatch = 0;
    b->id = 0;

    return b;
}

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->root = branch_init();
    dn->symbolHt = hash_table_init(500);
    //dn->symbolHt = hash_table_init(100);
    dn->idLookup = vector_init();
    vector_push_back(dn->idLookup, NULL);
    return dn;
}

hash_table* getSymbolHt(d_net* dn) {
    return dn->symbolHt;
}

int patternsAdded = 0;

void _add_pattern(d_net* dn, net_branch* b, flatterm* ft) {
    term* t = flatterm_first(ft);

    net_branch* subnet = NULL;
    while (t) {

        if ((subnet = find_subnet(b, t->id, t->m_type, t->f_type)) != NULL) {
            free(t->symbol);
            b = subnet;
        } else {
            if (t->m_type != MT_CONSTANT && t->id == vector_size(dn->idLookup)) {
                vector_push_back(dn->idLookup, t->symbol); //!This will get fucked atm...
            } /*else if (id > vector_size(dn->idLookup)) { //Remove me later
                fprintf(stderr, "ERROR: id > vector_size(dn->idLookup)\n");
                exit(1);
            }*/

            subnet = branch_init();
            subnet->symbol = t->symbol;
            subnet->m_type = t->m_type;
            subnet->f_type = t->f_type;
            subnet->id = t->id;
            vector_push_back(b->subnets, subnet);
            b = subnet;
        }

        t = t->next;
    }
    patternsAdded += 1;
    subnet->isMatch = 1;
    flatterm_free(ft);
}

void add_pattern(d_net* dn, const char str[]) {
    //fprintf(stderr, "%p\n", dn->symbolHt);
    flatterm* ft = parsePattern(str, dn->symbolHt);

    if (ft != NULL) {
        _add_pattern(dn, dn->root, ft);
    } else {
        fprintf(stderr, "failed at parsing flatterm for %s\n", str);
    }
    
}

int get_patterns_added() {
    return patternsAdded;
}

net_branch* find_subnet(net_branch* dn, int id, enum matchtype m_type, enum functype f_type) {
    void** sn_data = vector_data(dn->subnets);
    for (int i = 0; i < vector_size(dn->subnets); i++) {
        net_branch* sn = (net_branch*)sn_data[i];

        if (sn->m_type == m_type && f_type == sn->f_type && sn->id == id) { 
            return sn;
        }
    }

    return NULL;
}


int _print_net(net_branch* dn, int offset) {    
    int newBranch = 0;
    printf("%s(%d) ", dn->symbol, dn->id);
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
        printf(", isMatch: %d\n", dn->isMatch);
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
int is_valid_match(int patternId, subjectFlatterm* sf, int sfLen, sub_arr_entry* s_arr, bool fullNameIdMatching) {
    
    sub_arr_entry* entry = &s_arr[patternId];

    if (entry->len == 0) {

        // fprintf(stderr, "Trying to match to :");
        for (int i = 0; i < sfLen; i++) {
            
            // fprintf(stderr, "%s, ", sf->fullName[0]);
            sf = sf->skip;
        }
        // fprintf(stderr, "\n");
        return 2;
    }

    if (sfLen != entry->len) {
        return 0;
    }

    subjectFlatterm* sfMatch = entry->to;

    for (int i = 0; i < entry->len; i++) {

        if (fullNameIdMatching) {
            if (sfMatch->fullNameId != sf->fullNameId) {
                return 0;
            }
        } else {
            if (sfMatch->id != sf->id) {
                return 0;
            }
        }
        
        sfMatch = sfMatch->skip;
        sf = sf->skip;
    }
    return 1;
}
/*int is_valid_match(char* patternSymbol, subjectFlatterm* sf, int sfLen, int_vector* sv) {
    int foundValid = 2;

    for (size_t i = 0; i < int_vector_size(sv); i++) {
        s_entry* entry = int_vector_at(sv, i);
        
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
}*/

int get_strCmps() {
    return strCmps;
}

/*double time1 = 0;
clock_t time1Start;
clock_t time1End;

double time21 = 0;
clock_t time21Start;
clock_t time21End;

double time22 = 0;
clock_t time22Start;
clock_t time22End;

double time23 = 0;
clock_t time23Start;
clock_t time23End;

double time3 = 0;
clock_t time3Start;
clock_t time3End;*/

void _match(d_net* net, net_branch* branch, subjectFlatterm* t, sub_arr_entry* s_arr, int_vector* sv, vector* matches) {
//void _match(net_branch* branch, flatterm* subject, term* t, vector* sv, vector* matches) {
    if (t == NULL) {
        net_branch* subnet = NULL;
        void** subnet_data = vector_data(branch->subnets);
        for (int i = 0; i < vector_size(branch->subnets); i++) {
            //time1Start = clock();
            subnet = (net_branch*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->id, NULL, -1, s_arr, true)) {
                case 1:
                    /*time1End = clock();
                    time1 += (double)(time1End - time1Start) / CLOCKS_PER_SEC;*/
                    _match(net, subnet, t, s_arr, sv, matches);
                    break;
                case 2:
                    int_vector_push_back(sv, subnet->id);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = -1;
                    /*time1End = clock();
                    time1 += (double)(time1End - time1Start) / CLOCKS_PER_SEC;*/

                    _match(net, subnet, t, s_arr, sv, matches);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    int_vector_pop_back(sv);
                    break;
                case 0: //invalid match;
                    break;
                }
            }
        }

        if (branch->isMatch != 0) {
            // TODO add pattern pointer from net
            match_entry* new_match = create_match(NULL, sv, s_arr, net->idLookup);
            vector_push_back(matches, new_match);
        }
    } else {
        net_branch* subnet = NULL;
        enum functype f_type = t->f_type;

        void** subnet_data = vector_data(branch->subnets);
        for (int i = 0; i < vector_size(branch->subnets); i++) {
            //time21Start = clock();
            subnet = (net_branch*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                if (f_type == subnet->f_type) { 
                    if (subnet->id == t->id) {
                        /*time21End = clock();
                        time21 += (double)(time21End - time21Start) / CLOCKS_PER_SEC;*/
                        _match(net, subnet, t->next, s_arr, sv, matches);
                    }
                }
                continue;
            } else if (f_type == FT_PREFIX && subnet->m_type == MT_VARIABLE && subnet->f_type == FT_PREFIX) {
                switch (is_valid_match(subnet->id, t, 1, s_arr, false)) {
                case 1:
                    // Matching generic function name
                    /*time23End = clock();
                    time23 += (double)(time23End - time23Start) / CLOCKS_PER_SEC;*/
                    _match(net, subnet, t->next, s_arr, sv, matches);
                    break;
                case 2:
                    int_vector_push_back(sv, subnet->id);
                    s_arr[subnet->id].to = t;
                    s_arr[subnet->id].len = 1;
                    _match(net, subnet, t->next, s_arr, sv, matches);

                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    int_vector_pop_back(sv);
                    break;
                case 0: //invalid match;
                    break;
                }
                
                continue;
            } 
            //time22Start = clock();

            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->id, NULL, -1, s_arr, true)) {
                case 1:
                    /*time22End = clock();
                    time22 += (double)(time22End - time22Start) / CLOCKS_PER_SEC;*/
                    _match(net, subnet, t, s_arr, sv, matches);
                    break;
                case 2:
                    int_vector_push_back(sv, subnet->id);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = -1;
                    /*time22End = clock();
                    time22 += (double)(time22End - time22Start) / CLOCKS_PER_SEC;*/

                    _match(net, subnet, t, s_arr, sv, matches);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    int_vector_pop_back(sv);
                    break;
                case 0: //invalid match;
                    break;
                }
            } 
            //time23Start = clock();

            switch (is_valid_match(subnet->id, t, 1, s_arr, true)) {
            case 1:
                // Variable substitution matching, replace whole term.
                /*time23End = clock();
                time23 += (double)(time23End - time23Start) / CLOCKS_PER_SEC;*/
                _match(net, subnet, t->skip, s_arr, sv, matches);
                break;
            case 2:
                int_vector_push_back(sv, subnet->id);
                s_arr[subnet->id].to = t;
                s_arr[subnet->id].len = 1;

                // Variable substitution matching, replace whole term.
                /*time23End = clock();
                time23 += (double)(time23End - time23Start) / CLOCKS_PER_SEC;*/
                _match(net, subnet, t->skip, s_arr, sv, matches);

                s_arr[subnet->id].to = NULL;
                s_arr[subnet->id].len = 0;
                int_vector_pop_back(sv);
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
                        //time3Start = clock();
                        // t = slå ihop allt mellan nuvarande term och i
                        //matcha med den substitutionen
                        switch (is_valid_match(subnet->id, t, len, s_arr, true)) {
                        case 1:
                            /*time3End = clock();
                            time3 += (double)(time3End - time3Start) / CLOCKS_PER_SEC;*/
                            _match(net, subnet, tEnd->skip, s_arr, sv, matches);
                            break;
                        case 2:
                            int_vector_push_back(sv, subnet->id);
                            s_arr[subnet->id].to = t;
                            s_arr[subnet->id].len = len;
                            /*time3End = clock();
                            time3 += (double)(time3End - time3Start) / CLOCKS_PER_SEC;*/

                            _match(net, subnet, tEnd->skip, s_arr, sv, matches);
                            s_arr[subnet->id].to = NULL;
                            s_arr[subnet->id].len = 0;
                            int_vector_pop_back(sv);
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

/*void print_times() {
    fprintf(stderr, "time1: %f, time21: %f, time22: %f, time23: %f, time3: %f\n", time1, time21, time22, time23, time3);
}*/

match_set* pattern_match(d_net* dn, char* subject) {
    vector* matches = vector_init();
    int_vector* sv = int_vector_init();
    sub_arr_entry* s_arr = calloc(vector_size(dn->idLookup), sizeof(sub_arr_entry)); //Todo maybe move into dnet

	subjectFlatterm* ft_subject = parse_subject(subject, dn->symbolHt); //!Note if f[x + y]
	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, sv, matches);

    
    free(s_arr);
    int_vector_free(sv);
    return create_match_set(ft_subject, matches);
}

match_set* pattern_match_measure(d_net* dn, subjectFlatterm* ft_subject) {
    vector* matches = vector_init();
    int_vector* sv = int_vector_init();
    sub_arr_entry* s_arr = calloc(vector_size(dn->idLookup), sizeof(sub_arr_entry)); //Todo maybe move into dnet

	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, sv, matches);

    // subjectFlatterm_free(ft_subject); // !Note Cant free here because it is needed by results, how to handle this?
    free(s_arr);
    int_vector_free(sv);
    return create_match_set(ft_subject, matches);
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
    hash_table_free(net->symbolHt);
    vector_free(net->idLookup, NULL);
    free(net);
    
}