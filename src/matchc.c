
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "d_net.h"

int main(int argc, char* argv[]) {

	d_net* net = net_init();

	flatterm* ft_subject = parsePattern("f(2, c) := x");
	add_pattern(net, parsePattern("f(x_, a_) := x"));
	add_pattern(net, parsePattern("f(2, a_) := x"));
	printf("Net: \n");
	print_net(net);
	printf("-----\nMatches:\n");
	pattern_match(net, ft_subject);
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