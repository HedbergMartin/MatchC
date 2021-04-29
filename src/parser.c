#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "Datatypes/vector.h"


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

	if (begin != -1 && end == -1) {
		end = i;
	}

	expr->m_type = trailing_;
	expr->symbol = malloc(sizeof(char) * (end-begin-trailing_));
	memcpy(expr->symbol, str+begin, (end-begin-trailing_));
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
				expression* expr = malloc(sizeof(expression));
				int patternLen = parsePattern(str, expr, i);
				if (patternLen == -1) {
					return -1;
				}
				i = patternLen;
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
	while ((c = str[i]) != '\0') {
		if (top) {
			if (c == ':') {
				if (str[i+1] == '=') {
					return i;
				}
				return -1;
			} else if (c != ' ') {
				return -1;
			}
		} else {
			if (c == ',' || c == ')') {
				return i;
			} else if (c != ' ') {
				return -1;
			}
		}

		i++;
	}

	return -1;
}

int parsePattern(const char str[], expression* expr, int i) {
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


	printf("Symbol: %s, type: %s, matching: %s\n", expr->symbol, (expr->f_type ? "prefix" : "symbol"), (expr->m_type ? "variable" : "constant"));
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
	expression* expr = malloc(sizeof(expression));
	parsePattern(str, expr, 0);

}

// int readExpression(char str[], char terminators[], int* lenght) {
// 	// enum ExpressionType expType = UNASSIGNED;
// 	int i = 0;
// 	while (1) {
// 		if (terminates(terminators, str[i])) {
// 			//Expression is done
// 			*lenght = i;
// 			return 0;
// 		} else if (str[i] == '(') {
// 			int len;
// 			int arity;
// 			if (readFunctionArgs(str+i+1, &len, &arity)) {
// 				return 1;
// 			}
// 			i += len + 1; // + 1 Skip closing bracket
// 		} else {
// 			i++;
// 		}
// 	}
// }

// int readFunctionArgs(char str[], int* lenght, int* arity) {
// 	int start = 0;

// 	int i = 0;
// 	*arity = 0;
// 	while(1) {
// 		if (str[i] == ')') {
// 			*lenght = i+1;
// 			return 0;
// 		} else if (str[i] == ',') {
// 			i++;
// 		} else if (str[i] == '\0') {
// 			return 1;
// 		}

// 		int len;
// 		if (readExpression(str+i, ",)", &len)) {
// 			return 1; //Error
// 		}
// 		(*arity)++;
// 		i += len;
// 	}

// 	return i;
// }
