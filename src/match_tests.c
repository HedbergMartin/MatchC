#include "ihct.h"
#include "d_net.h"
#include "parser.h"
#include "string.h"
#include "stdio.h"

int test_net(char* patterns[], char* subject, char* shouldMatch[]) {
	subjectFlatterm* ft_subject = parse_subject(subject);
	d_net* net = net_init();

    int i = 0;
    while (patterns[i] != NULL) {
        flatterm* ft = parsePattern(patterns[i]);
        add_pattern(net, ft);
        i++;
    }
	
	vector* matches = pattern_match(net, ft_subject);

    int res = 0;
	for (i = 0; i < vector_size(matches); i++) {
        char* m = (char*)vector_at(matches, i);
        // printf("%s == %s\n", shouldMatch[i], m);
        if (strcmp(shouldMatch[i], m) != 0) {
            res = 1;
            break;
        }
	}

	vector_free(matches, free);
    return res;
}

IHCT_TEST(variable_match) {
    char* patterns[] = {"f[x_]", NULL};
    char* matches[] = {"(x_ -> 2) ", NULL}; //Remember trailing whitespace

    IHCT_ASSERT(test_net(patterns, "f[2]", matches) == 0);
}

IHCT_TEST(variable_function_match) {
    char* patterns[] = {"f[x_]", NULL};
    char* matches[] = {"(x_ -> g) ", NULL}; //Remember trailing whitespace

    IHCT_ASSERT(test_net(patterns, "f[g[h[]]]", matches) == 0);
}

IHCT_TEST(sequence_match) {
    char* patterns[] = {"f[x___, y___]", NULL};
    char* matches[] = { "(x___ -> #) (y___ -> a, b, c) ", 
                        "(x___ -> a) (y___ -> b, c) ", 
                        "(x___ -> a, b) (y___ -> c) ", 
                        "(x___ -> a, b, c) (y___ -> #) ", NULL}; //Remember trailing whitespace

    IHCT_ASSERT(test_net(patterns, "f[a, b, c]", matches) == 0);
}

int main(int argc, char **argv) {
    return IHCT_RUN(argc, argv);
}