
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "d_net.h"

int main(int argc, char* argv[]) {

	d_net* net = net_init();

	//subjectFlatterm* ft_subject = parse_subject("f[  p  ,   b  [qasd   [ 3 ], a], c, 1]");
	subjectFlatterm* ft_subject = parse_subject("f[u[p]]"); //!Note if f[x + y]
	//flatterm* ft = parsePattern("f[a_,b_[q[a_], t_],c_ + p_, d_] := x");
	//flatterm* ft = parsePattern("f[p_[c[]], t_ + 4 * d] := x");
  //flatterm* ft_subject = parsePattern("f(a, 2, c) := x");
	//add_pattern(net, parsePattern("f(x_(2), y_) := x"));
	flatterm* ft = parsePattern("f[u[y_]] := x");	//!Remove :=
	flatterm_print(ft);
	fprintf(stderr, "done\n");
	//add_pattern(net, parsePattern("f[x___, y__] := x"));
	add_pattern(net, ft);
	
	// add_pattern(net, parsePattern("f(2, a_) := x"));
	printf("Net: \n");
	print_net(net);
	printf("-----\nMatches:\n");
	vector* matches = pattern_match(net, ft_subject);

	for (int i = 0; i < vector_size(matches); i++) {
		net_match* match = (net_match*)vector_at(matches, i);
		printf("MatchID: %d\n", match->matchid);
		for (int i = 0; i < match->subst_amount; i++) {
			s_entry* su = &(match->substitutions[i]);
			printf("From: %s, To: ", su->from);
			for (int j = 0; j < su->len; j++) {
				subjectFlatterm* ft = &(su->to[j]);
				printf("%s", ft->symbol);
			}
			printf("\n");
		}
	}

	vector_free(matches, free);
	
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