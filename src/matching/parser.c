#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include <time.h>
#include "vector.h"


int isAcceptedCharacter(char c);
int _parsePattern(const char str[], flatterm* ft, int i);

term* term_create() {
	term* t = malloc(sizeof(term));
	if (!t) {
		perror("term");
		exit(1);
	}

	t->m_type = -1;
	t->f_type = -1;
	t->symbol = NULL;
	t->end = NULL;

	return t;
}

// void printFT(list* flatterm) {
// 	if (!list_isEmpty(flatterm)) {
// 		list_pos pos = list_first(flatterm);

// 		while (1) {
// 			term* t = list_inspect(flatterm, pos);
// 			printf("%p ", t->end);

// 			if(list_isEnd(flatterm, pos)) {
// 				printf("\n");
// 				break;
// 			}
// 			pos = list_next(flatterm, pos);
// 		}
// 	}
// }

void parseSubject(char* expression) {

}

int terminates(char c, char terminators[]) {
	if (c == '\0') {
		return 1;
	}

	int i = 0;
	while (terminators[i] != '\0') {
		if (terminators[i] == c) {
			return 1;
		}
		i++;
	}

	return 0;
}

int readSymbol(const char str[], int i, term* t) {
	int begin = -1;
	int end = -1;

	int trailing_ = 0;

	char c;
	while (!terminates((c = str[i]), ",():")) {
		if (c == ' ') {
			if (begin != -1 && end == -1) {
				end = i;
			}
		} else if (isAcceptedCharacter(c)) {
			if (begin == -1) {
				begin = i;
			} else if (end != -1) {
				return -1;
			}

			if (c == '_') {
				trailing_++;
			} else {
				trailing_ = 0;
			}

		} else {
			return -1;
		}
		i++;
	}

	if (begin == -1) {
		return -1;
	}

	if (begin != -1 && end == -1) {
		end = i;
	}

	t->m_type = trailing_;
	int len = (end-begin-trailing_);
	t->symbol = malloc(sizeof(char) * len + 1);
	memcpy(t->symbol, str+begin, len);
	t->symbol[len] = '\0';
	//printf("Name: %.*s\n", end-begin, str+begin);
	return i;
}

int isAcceptedCharacter(char c) {
	if ((c >= 'A' && c <= 'Z') || 
		(c >= 'a' && c <= 'z') || 
		(c >= '0' && c <= '9') || 
		c == '_') {
		return 1;
	}

	return 0;
}

int readArgs(const char str[], int i, flatterm* ft, term* t) {
	if (str[i] == '(') {
		t->f_type = FT_PREFIX;
		i++;

		char c;
		while ((c = str[i]) != '\0') {
			if (c == ')') {
				t->end = flatterm_end(ft);
				return i+1;
			} else {
				int patternLen = _parsePattern(str, ft, i);
				if (patternLen == -1) {
					return -1;
				}

				i = patternLen;
				if (str[i] == ',') {
					i++;
				}
				while (str[i] == ' ') {
					i++;
				}
			}
		}

		return -1;
	} else {
		t->f_type = FT_NOTAFUNC;
		t->end = flatterm_end(ft);
		return i;
	}
}

int parseTail(const char str[], int i, int top) {
	char c;
	if (top) {
		while ((c = str[i]) != '\0') {
			if (c == ':') {
				if (str[i+1] == '=') {
					return i;
				}
				return -1;
			} else if (c != ' ') {
				return -1;
			}

			i++;
		}
	} else {
		while ((c = str[i]) != '\0') {
			if (c == ',' || c == ')') {
				return i;
			} else if (c != ' ') {
				return -1;
			}

			i++;
		}
	}

	return -1;
}

int _parsePattern(const char str[], flatterm* ft, int i) {
	int top = 0;
	if (i == 0) {
		top = 1;
	}

	term* t = flatterm_push_back(ft);

	int symbolEnd = readSymbol(str, i, t);
	if (symbolEnd == -1) {
		fprintf(stderr, "Error parsing a symbol in expression \"%s\"\n", str);
		return -1;
	}
	i = symbolEnd;
	
	int argsEnd = readArgs(str, i, ft, t);
	if (argsEnd == -1) {
		fprintf(stderr, "Error parsing an argument in expression \"%s\"\n", str);
		return -1;
	}
	i = argsEnd;

	int tailEnd = parseTail(str, i, top);
	if (tailEnd == -1) {
		fprintf(stderr, "Error parsing expression \"%s\"\n", str);
		return -1;
	}
	i = tailEnd;

	// if (expr->f_type == FT_NOTAFUNC) {
	// 	printf("Symbol: matchtype: %d, symbol: %s\n", expr->m_type, expr->symbol);
	// } else if (expr->f_type == FT_PREFIX) {
	// 	printf("Prefix func: matchtype: %d, symbol: %s\n", expr->m_type, expr->symbol);
	// } else {
	// 	printf("Error\n");
	// }

	// printf("%p\n", (list_inspect(flatterm, t->end)));
	return i;
}

void debugPattern(const char str[]) {

    clock_t time;

    time = clock();
	flatterm* ft = parsePattern(str);
    time = clock() - time;

    double time_taken = (((double)time)/CLOCKS_PER_SEC)*1000;
  
    printf("took %fms\n", time_taken);
	if (ft != NULL) {
		flatterm_print(ft);
	}
	
}

flatterm* parsePattern(const char str[]) {
	flatterm* ft = flatterm_init();
	int res = _parsePattern(str, ft, 0);
	if (res == -1) {
		printf("Error"); //TODO Errormessage and free of resources!
		return NULL;
	}
	return ft;
}

// void printExpr(flatterm* ft) {
// 	list_pos ft_pos = list_first(flatterm);

// 	while (1) {
// 		term* t = list_inspect(flatterm, ft_pos);
// 		term* endTerm = list_inspect(flatterm, t->end);
// 		printf("Symbol: %s, type: %s, matching: %s, end: %p\n", t->symbol, (t->f_type ? "prefix" : "symbol"), (t->m_type ? "variable" : "constant"), t->end);

// 		if (list_isEnd(flatterm, ft_pos)) {
// 			return;
// 		}
// 		ft_pos = list_next(flatterm, ft_pos);
// 	}
	
// }
