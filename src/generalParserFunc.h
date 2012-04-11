/**
 * @file generalParserFunc.h
 *
 *  Created on: Apr 10, 2012
 *      Author: DirkK
 */
#ifndef GENERALPARSERFUNC_H_
#define GENERALPARSERFUNC_H_

#define GETLISTHEAD(list, result) \
	result = list; \
	while(result->prev != NULL) { \
		result = result->prev; \
	} \

expr_t expressionReturn(expr_t children);

#endif


