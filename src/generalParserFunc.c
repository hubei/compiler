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
 * Hint: You should free the string later!!
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
		// lets reserve memory for an empty string so that we can free
		// it without problems later :)
		res = malloc(1);
		strcpy(res, "");
	}
	return res;
}

/**
 * @brief free a statement. Not very spectacular yet ;)
 * @param stmt
 */
void clean_stmt(stmt_t* stmt) {
	free(stmt);
}

/**
 * @brief loop through index list a free each entry
 * @param il
 */
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
