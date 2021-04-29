
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

	char* str = "f(x_) := x";
	// debugPattern(str);
	str = "f_(x_) := x";
	debugPattern(str);
	// str = "f_ := x";
	// debugPattern(str);
	str = "f_(h(g)) := x";
	debugPattern(str);
}