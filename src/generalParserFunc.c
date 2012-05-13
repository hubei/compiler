/*
 * @file generalParserFunc.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "types.h"
#include <stdio.h>

/**
 * TODO Dirk maybe remove? ^^
 * @param children
 */
void expressionReturn(expr_t* children) {
	children->lvalue = 0;
}

/**
 * @brief Return a string representation of a value (id or num)
 * @param arg argument to convert
 * @return string (empty string, if sth is NULL)
 */
char* valueAsString(expr_t* arg) {
	char* res = NULL;
	if (arg != NULL) {
		switch (arg->valueKind) {
		case VAL_ID:
			res = arg->value.id;
			break;
		case VAL_NUM:
			res = malloc(11); // int has max 10 digits + end of string
			if (res == NULL) {
				// TODO error
			}
			sprintf(res, "%d", arg->value.num);
			break;
		case VAL_UNKOWN:
			break;
		}
	}
	if (res == NULL) {
		res = malloc(1);
		strcpy(res, "");
	}
	return res;
}
