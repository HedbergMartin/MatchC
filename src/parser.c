#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

int isAcceptedCharacter(char c);

void parseSubject(char* expression) {

}

int readName(char str[]) {
	int begin = -1;
	int end = -1;

	int i = 0;
	char c;
	while ((c = str[i]) != '\0') {
		if (c == ' ') {
			if (begin != -1 && end == -1) {
				end = i;
			}
		} else if (isAcceptedCharacter(c)) {
			if (begin == -1) {
				begin = i;
			} else if (end != -1) {
				return 1;
			}
		} else {
			return 1;
		}
		i++;
	}

	printf("Name: %.*s\n", end-begin, str+begin);
	return 0;
}

int isAcceptedCharacter(char c) {
	if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) {
		return 1;
	}

	return 0;
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
