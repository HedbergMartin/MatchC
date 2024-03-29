
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "d_net.h"
#include "match_entry.h"

int main(int argc, char* argv[]) {

	d_net* net = net_init();

	//subjectFlatterm* ft_subject = parse_subject("f[  p  ,   b  [qasd   [ 3 ], a], c, 1]");
	//flatterm* ft = parsePattern("f[a_,b_[q[a_], t_],c_ + p_, d_] := x");
	//flatterm* ft = parsePattern("f[p_[c[]], t_ + 4 * d] := x");
  //flatterm* ft_subject = parsePattern("f(a, 2, c) := x");
	//add_pattern(net, parsePattern("f(x_(2), y_) := x"));
	//hash_table* ht_constants = hash_table_init(100);
	//flatterm* ft = parsePattern("f[x_, x_]", ht_constants);	//!Remove :=
	// flatterm_print(ft);
	// fprintf(stderr, "done\n");
	//add_pattern(net, parsePattern("f[x___, y__] := x"));
	add_pattern(net, "f[x_]");
	print_net(net);
	
	//printf("Net: \n");
	//print_net(net);
	match_set* matches = pattern_match(net, "f[2]");
	printf("\n-----\nMatches:\n");

	for (int i = 0; i < matches_size(matches); i++) {
		match_entry* match = get_match(matches, i);
		printf("MatchID: %s\n", match->pattern);
		for (int k = 0; k < match->subst_amount; k++) {
			substitution* su = &(match->substitutions[k]);
			printf("From: %s, To: ", su->from);
			char** ft = su->to;
			for (int j = 0; j < su->len; j++) {
				printf("%s, ", ft[j]);
			}
			printf("\n");
		}
	}

	match_set_free(matches);
	net_free(net);
	
	// debugPattern(str);
	// str = "f_(x_) := x";
	// debugPattern(str);
	// str = "f_ := x";
	// debugPattern(str);
	// str = "f_(h(g), j_, p(), a, b, c, d) := x";
	// list* ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "f_(h(g), j_, k, g, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "f_(h(g), j_, as, g, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "f_(h(g), j_, as, i, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "f_(h(g), i, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "fuu_(h(g), i, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }
	// str = "fuu_(h(g), a, b) := x";
	// ft = parsePattern(str);
	// if (ft != NULL) {
	// 	add_pattern(net, ft);
	// }

	//print_net(net);

	// list* subj = parsePattern("f(h(g), j, as, g, b) :=");
	// debugPattern("f_(h(g), j_, p(), a(b, c), d) := x");

	// flatterm* ft = flatterm_init();
	// term* t1 = flatterm_push_back(ft);
	// t1->symbol = "A";
	// term* t2 = flatterm_push_back(ft);
	// t2->symbol = "B";
	// term* t3 = flatterm_push_back(ft);
	// t3->symbol = "C";
	// term* t4 = flatterm_push_back(ft);
	// t4->symbol = "D";

	// flatterm_print(ft);

	//pattern_match(net, subj);

	//str = "f(,) := x";
	// debugPattern(str);
}