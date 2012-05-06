/*
 * @file generalParserFunc.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "types.h"
#include <err.h>
#include <stdio.h>

void expressionReturn(expr_t* children) {
	children->lvalue = 0;
}

char* valueAsString(expr_t* arg) {
	char* res = NULL;
	if(arg == NULL) {
		return NULL;
	}
	switch (arg->valueKind) {
	case VAL_ID:
		res = arg->value.id;
		break;
	case VAL_NUM:
		res = malloc(11); // int has max 10 digits + end of string
		if (res == NULL)
			err(1, "Could not allocate");
		sprintf(res, "%d", arg->value.num);
		break;
	default:
		res = "";
		break;
	}
	assert(res!=NULL);
	return res;
}
