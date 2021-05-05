#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <errno.h>

#include <time.h>
#include "vector.h"


enum matchtype {
	MT_CONSTANT, // 2 or f
	MT_VARIABLE, // f_ or x_
	MT_SEQUENCE // f__ or x__
};

enum functype {
	FT_NOTAFUNC,
	FT_PREFIX,
	FT_INFIX
};

typedef struct expression {
	enum matchtype m_type;
	enum functype f_type;
	char* symbol;
	vector* params;
} expression;


int isAcceptedCharacter(char c);
int _parsePattern(const char str[], expression* expr, int i);
void printExpr(expression* expr, int level);
void expression_free(expression* expr);

expression* expr_create() {
	expression* expr = malloc(sizeof(expression));
	if (!expr) {
		perror("expr");
		exit(1);
	}

	expr->m_type = -1;
	expr->f_type = -1;
	expr->symbol = NULL;
	expr->params = NULL;

	return expr;
}

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

int readSymbol(const char str[], int i, expression* expr) {
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

	expr->m_type = trailing_;
	int len = (end-begin-trailing_);
	expr->symbol = malloc(sizeof(char) * len + 1);
	memcpy(expr->symbol, str+begin, len);
	expr->symbol[len] = '\0';
	//printf("Name: %.*s\n", end-begin, str+begin);
	return i;
}

int isAcceptedCharacter(char c) {
	if ((c >= 65 && c <= 90) || 
		(c >= 97 && c <= 122) || 
		c == '_') {
		return 1;
	}

	return 0;
}

int readArgs(const char str[], int i, expression* expr) {
	if (str[i] == '(') {
		expr->f_type = FT_PREFIX;
		i++;

		char c;
		while ((c = str[i]) != '\0') {
			if (c == ')') {
				return i+1;
			} else {
				expression* childExpr = expr_create();
				int patternLen = _parsePattern(str, childExpr, i);
				if (patternLen == -1) {
					return -1;
				}
				if (expr->params == NULL) {
					expr->params = vector_init();
				}
				vector_push_back(expr->params, childExpr);
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
		expr->f_type = FT_NOTAFUNC;
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

int _parsePattern(const char str[], expression* expr, int i) {
	int top = 0;
	if (i == 0) {
		top = 1;
	}

	int symbolEnd = readSymbol(str, i, expr);
	if (symbolEnd == -1) {
		fprintf(stderr, "Error parsing a symbol in expression \"%s\"\n", str);
		return -1;
	}
	i = symbolEnd;
	
	int argsEnd = readArgs(str, i, expr);
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

	return i;
}

void debugPattern(const char str[]) {

    clock_t t;

    t = clock();
	expression* expr = expr_create();
	int res = _parsePattern(str, expr, 0);
    t = clock() - t;

    double time_taken = (((double)t)/CLOCKS_PER_SEC)*1000;
  
    printf("took %fms\n", time_taken);
	if (res != -1) {
		printExpr(expr, 0);
	}
	expression_free(expr);
}

void expression_free(expression* expr) {
	if (expr != NULL) {
		if (expr->params != NULL) {
			vector_free(expr->params, (void (*)(void *))expression_free);
		}

		if (expr->symbol) {
			free(expr->symbol);
		}
		free(expr);
	}
}

void printExpr(expression* expr, int level) {
	for (int j = 0; j < level; j++) {
		printf("	");
	}
	printf("Symbol: %s, type: %s, matching: %s\n", expr->symbol, (expr->f_type ? "prefix" : "symbol"), (expr->m_type ? "variable" : "constant"));
	
	if (expr->params != NULL) {
		expression** data = (expression**) vector_data(expr->params);
		for (int i = 0; i < vector_size(expr->params); i++) {
			printExpr(data[i], level + 1);
		}
	}
}
