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
    flatterm* match;
} net_branch;

//!Make sure to find max variable depth
struct d_net {
    net_branch* root;
    hash_table* symbolHt;
    int max_depth;
    //vector* idLookup;
};

net_branch* find_subnet(net_branch* dn, int id, enum matchtype m_type, enum functype f_type);

net_branch* branch_init() {
    net_branch* b = malloc(sizeof(net_branch));
    b->subnets = vector_init();
    b->symbol = "";
    b->match = NULL;
    b->id = 0;

    return b;
}

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->root = branch_init();
    dn->symbolHt = hash_table_init(100);
    dn->max_depth = 0;
    //dn->symbolHt = hash_table_init(100);
    //dn->idLookup = vector_init();
    //vector_push_back(dn->idLookup, NULL);
    return dn;
}

hash_table* getSymbolHt(d_net* dn) {
    return dn->symbolHt;
}

int patternsAdded = 0;

/**
 * @brief 
 * 
 * @param dn 
 * @param b 
 * @param ft 
 * @return flatterm* already existing identical flatterm or NULL if new flatterm
 */
flatterm* _add_pattern(d_net* dn, net_branch* b, flatterm* ft) {
    term* t = flatterm_first(ft);
    int current_depth = 0;

    net_branch* subnet = NULL;
    while (t) {

        if ((subnet = find_subnet(b, t->id, t->m_type, t->f_type)) != NULL) {
            //free(t->symbol);
            b = subnet;
        } else {
            /*if (t->m_type != MT_CONSTANT && t->id == vector_size(dn->idLookup)) {
                //vector_push_back(dn->idLookup, t->symbol); //!This will get fucked atm...
            } else if (id > vector_size(dn->idLookup)) { //Remove me later
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
        current_depth += 1;
        t = t->next;
    }
    patternsAdded += 1;
    //subnet->isMatch = 1;
    if (current_depth >= dn->max_depth) {
        dn->max_depth = current_depth;
    }

    if (b->match != NULL) {
        flatterm_free(ft);
        return b->match;
    } else {
        b->match = ft;
        return NULL;
    }
}

void add_pattern(d_net* dn, const char str[]) {
    //fprintf(stderr, "%p\n", dn->symbolHt);
    flatterm* ft = parsePattern(str, dn->symbolHt);

    if (ft != NULL) {
        flatterm* matchft = _add_pattern(dn, dn->root, ft);

        if (matchft != NULL) {
            /*fprintf(stderr, "\nNet already has logically identical pattern to: %s \nAlready entered: ", str);
            flatterm_print(matchft);*/
        }
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

        if (sn->id == id && sn->m_type == m_type && f_type == sn->f_type) { 
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

/**
 * return int 0 for invalid, 1 for existing, 2 for new
 */
int is_valid_match(int patternId, subjectFlatterm* sf, int sfLen, sub_arr_entry* s_arr, bool fullNameIdMatching) {
    sub_arr_entry* entry = &s_arr[patternId];
    

    if (entry->len == 0) {
        
        for (int i = 0; i < sfLen; i++) {
            sf = sf->skip;
        }
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

//void _match(d_net* net, net_branch* branch, subjectFlatterm* t, sub_arr_entry* s_arr, int_vector* sv, vector* matches) {
void _match(d_net* net, net_branch* branch, subjectFlatterm* t, sub_arr_entry* s_arr, vector* matches, int max_branch_id) {

    if (branch->id > max_branch_id && branch->m_type != MT_CONSTANT) {
        max_branch_id = branch->id;
    }
    if (t == NULL) {
        net_branch* subnet = NULL;
        void** subnet_data = vector_data(branch->subnets);
        for (int i = 0; i < vector_size(branch->subnets); i++) {
            subnet = (net_branch*)subnet_data[i];
            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->id, NULL, -1, s_arr, true)) {
                case 1:
                    _match(net, subnet, t, s_arr, matches, max_branch_id);
                    break;
                case 2:
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = -1;

                    _match(net, subnet, t, s_arr, matches, max_branch_id);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    break;
                case 0: //invalid match;
                    break;
                }
            }
        }

        if (branch->match != NULL) {
            // TODO add pattern pointer from net
            //fprintf(stderr, "Found a match %d\n");

            //for (int i = 0; i < branch->id)
            match_entry* new_match = create_match(NULL, s_arr, branch->match, max_branch_id);
            vector_push_back(matches, new_match);
        }
    } else {
        net_branch* subnet = NULL;
        enum functype f_type = t->f_type;

        void** subnet_data = vector_data(branch->subnets);
        for (int i = 0; i < vector_size(branch->subnets); i++) {
            subnet = (net_branch*)subnet_data[i];

            if (subnet->m_type == MT_CONSTANT) {
                if (f_type == subnet->f_type) { 
                    if (subnet->id == t->id) {
                        _match(net, subnet, t->next, s_arr, matches, max_branch_id);
                    }
                }
                continue;
            } else if (f_type == FT_PREFIX && subnet->m_type == MT_VARIABLE && subnet->f_type == FT_PREFIX) {
                switch (is_valid_match(subnet->id, t, 1, s_arr, false)) {
                case 1:
                    // Matching generic function name
                    _match(net, subnet, t->next, s_arr, matches, max_branch_id);
                    break;
                case 2:
                    s_arr[subnet->id].to = t;
                    s_arr[subnet->id].len = 1;
                    _match(net, subnet, t->next, s_arr, matches, max_branch_id);

                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    break;
                case 0: //invalid match;
                    break;
                }
                
                continue;
            } 

            if (subnet->m_type == MT_STAR) {

                switch (is_valid_match(subnet->id, NULL, -1, s_arr, true)) {
                case 1:
                    _match(net, subnet, t, s_arr, matches, max_branch_id);
                    break;
                case 2:
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = -1;

                    _match(net, subnet, t, s_arr, matches, max_branch_id);
                    s_arr[subnet->id].to = NULL;
                    s_arr[subnet->id].len = 0;
                    break;
                case 0: //invalid match;
                    break;
                }
            } 

            switch (is_valid_match(subnet->id, t, 1, s_arr, true)) {
            case 1:
                // Variable substitution matching, replace whole term.
                _match(net, subnet, t->skip, s_arr, matches, max_branch_id);
                break;
            case 2:
                //fprintf(stderr, "setting %d to %p\n", subnet->id, t);
                s_arr[subnet->id].to = t;
                s_arr[subnet->id].len = 1;

                // Variable substitution matching, replace whole term.
                _match(net, subnet, t->skip, s_arr, matches, max_branch_id);

                s_arr[subnet->id].to = NULL;
                s_arr[subnet->id].len = 0;
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
                        switch (is_valid_match(subnet->id, t, len, s_arr, true)) {
                        case 1:
                            _match(net, subnet, tEnd->skip, s_arr, matches, max_branch_id);
                            break;
                        case 2:
                            s_arr[subnet->id].to = t;
                            s_arr[subnet->id].len = len;

                            _match(net, subnet, tEnd->skip, s_arr, matches, max_branch_id);
                            s_arr[subnet->id].to = NULL;
                            s_arr[subnet->id].len = 0;
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
    sub_arr_entry* s_arr = calloc(dn->max_depth, sizeof(sub_arr_entry)); //Todo maybe move into dnet

	subjectFlatterm* ft_subject = parse_subject(subject, dn->symbolHt); //!Note if f[x + y]
	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, matches, 0);

    free(s_arr);
    int_vector_free(sv);
    return create_match_set(ft_subject, matches);
}

match_set* pattern_match_measure(d_net* dn, subjectFlatterm* ft_subject) {
    vector* matches = vector_init();
    int_vector* sv = int_vector_init();
    sub_arr_entry* s_arr = calloc(dn->max_depth, sizeof(sub_arr_entry)); //Todo maybe move into dnet
	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, matches, 0);

    // subjectFlatterm_free(ft_subject); // !Note Cant free here because it is needed by results, how to handle this?
    free(s_arr);
    int_vector_free(sv);
    return create_match_set(ft_subject, matches);
}


void _branch_free(void* netVoid) {
    net_branch* net = (net_branch*)netVoid;
    vector_free(net->subnets, _branch_free);
    //free(net->symbol);
    if (net->match != NULL) {
        flatterm_free(net->match);
    }
    free(net);   
}

void net_free(d_net* net) {
    vector_free(net->root->subnets, _branch_free);
    free(net->root);
    hash_table_free(net->symbolHt);
    //vector_free(net->idLookup, NULL);
    free(net);
    
}