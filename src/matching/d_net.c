#include "d_net.h"
#include "time.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "subjectFlatterm.h"
#include "match_entry.h"
#include "parser.h"
#include "net_branch.h"

#include "match_types.h"

//!Make sure to find max variable depth
struct d_net {
    net_branch* root;
    hash_table* symbolHt;
    int max_variables;
    int totalPatterns;
};

net_branch* find_subnet(net_branch* dn, int id, enum matchtype m_type, enum functype f_type);

d_net* net_init() {
    d_net* dn = malloc(sizeof(d_net));
    dn->root = net_init_root();
    dn->symbolHt = hash_table_init(100, 1);
    dn->max_variables = 0;

    return dn;
}

hash_table* getSymbolHt(d_net* dn) {
    return dn->symbolHt;
}

/**
 * @brief 
 * 
 * @param dn 
 * @param b 
 * @param ft 
 * @return flatterm* already existing identical flatterm or NULL if new flatterm
 */
void _add_pattern(d_net* dn, net_branch* b, flatterm* ft) {
    term* t = flatterm_first(ft);
    int current_depth = 0;

    net_branch* subnet = NULL;
    while (t) {
        if (t->m_type != MT_CONSTANT) {
            current_depth++;
        }

        if ((subnet = find_subnet(b, t->id, t->m_type, t->f_type)) != NULL) {
            //free(t->symbol);
            b = subnet;
        } else {

            b = net_branch_add(b, t->id, t->m_type, t->f_type);
        }
        t = t->next;
    }
    //subnet->isMatch = 1;
    if (current_depth > dn->max_variables) {
        dn->max_variables = current_depth;
    }

    if (b->match_data != NULL) {
        flatterm_free(ft);
    } else {
        net_branch_set_match(b, flatterm_pattern(ft), ft);
    }
}

void add_pattern(d_net* dn, char str[]) {
    flatterm* ft = parsePattern(str, dn->symbolHt);

    if (ft != NULL) {
        _add_pattern(dn, dn->root, ft);
    } else {
        fprintf(stderr, "failed at parsing flatterm for %s\n", str);
    }
    
}

int get_patterns_added(d_net* net) {
    return net->totalPatterns;
}

int _print_net(net_branch* dn, int offset) {    
    int newBranch = 0;
    printf("%d ", dn->id);
    for (int i = 0; i < net_branch_size(dn); i++) {
        if (newBranch) {
            for (int i = 0; i <= offset; i++) {
                printf(" ");
            }
        }
        net_branch* sn = net_branch_sub_branch(dn, i);
        newBranch = _print_net(sn, offset + 1);
    }
    if (dn->match_data != NULL) {
        printf(", match!\n");
        return 1;
    }
    return newBranch;
}

void print_net(d_net* dn) {
    net_branch* b = dn->root;
    for (int i = 0; i < net_branch_size(b); i++) {
        net_branch* sn = net_branch_sub_branch(b, i);
        _print_net(sn, 0);
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

void _match(d_net* net, net_branch* branch, subjectFlatterm* t, sub_arr_entry* s_arr, vector* matches, int max_branch_id) {

    if (branch->id > max_branch_id && branch->m_type != MT_CONSTANT) {
        max_branch_id = branch->id;
    }
    
    if (t == NULL) {
        net_branch* subnet = NULL;
        for (int i = 0; i < net_branch_size(branch); i++) {
            subnet = net_branch_sub_branch(branch, i);
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

        if (branch->match_data != NULL) {
            match_entry* new_match = create_match(branch->match_data->pattern, s_arr, branch->match_data->ft, max_branch_id);
            vector_push_back(matches, new_match);
        }
    } else {
        net_branch* subnet = NULL;
        enum functype f_type = t->f_type;

        for (int i = 0; i < net_branch_size(branch); i++) {
            subnet = net_branch_sub_branch(branch, i);

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
    sub_arr_entry* s_arr = calloc(dn->max_variables + 1, sizeof(sub_arr_entry)); //Todo maybe move into dnet

	subjectFlatterm* ft_subject = parse_subject(subject, dn->symbolHt);
	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, matches, 0);

    free(s_arr);
    return create_match_set(ft_subject, matches);
}

match_set* pattern_match_measure(d_net* dn, subjectFlatterm* ft_subject) {
    vector* matches = vector_init();
    sub_arr_entry* s_arr = calloc(dn->max_variables + 1, sizeof(sub_arr_entry)); //Todo maybe move into dnet
	// print_subjectFlatterm(ft_subject);

    _match(dn, dn->root, ft_subject, s_arr, matches, 0);

    free(s_arr);
    return create_match_set(ft_subject, matches);
}

void net_free(d_net* net) {
    net_root_free(net->root);
    hash_table_free(net->symbolHt);
    free(net);
}