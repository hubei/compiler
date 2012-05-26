/**
 * @file generalParserFunc.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "types.h"
#include <stdio.h>
#include "generalParserFunc.h"

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
			res = malloc(strlen(arg->value.id) + 1);
			strcpy(res, arg->value.id);
			break;
		case VAL_NUM:
			res = malloc(11); // int has max 10 digits + end of string
			if (res == NULL) {
				// TODO error memory
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

void clean_stmt(stmt_t* stmt) {
	free(stmt);
}

void clean_indexList(indexList_t* il) {
	if(il == NULL)
		return;
	indexList_t* tmp = NULL;
	GETLISTHEAD(il, tmp);
	il = tmp;
	while(il != NULL) {
		tmp = il;
		il = tmp->next;
		free(tmp);
	}
}
