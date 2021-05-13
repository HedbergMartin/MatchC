
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "d_net.h"

int main(int argc, char* argv[]) {

	d_net* net = net_init();

	char* str = "f(x_) := x";
	// debugPattern(str);
	// str = "f_(x_) := x";
	// debugPattern(str);
	// str = "f_ := x";
	// debugPattern(str);
	str = "f_(h(g), j_, p(), a, b, c, d) := x";
	list* ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "f_(h(g), j_, k, g, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "f_(h(g), j_, as, g, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "f_(h(g), j_, as, i, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "f_(h(g), i, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "fuu_(h(g), i, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}
	str = "fuu_(h(g), a, b) := x";
	ft = parsePattern(str);
	if (ft != NULL) {
		add_pattern(net, ft);
	}

	print_net(net);
	//str = "f(,) := x";
	// debugPattern(str);
}