/**
 * @file generalParserFunc.h
 *
 *  Created on: Apr 10, 2012
 *      Author: DirkK
 */
#ifndef GENERALPARSERFUNC_H_
#define GENERALPARSERFUNC_H_

/**
 * @brief Loads head of list into result
 * @param list pointer to any element in a list
 * @param result pointer to the resulting head of the list
 * @author NicolaiO
 */
#define GETLISTHEAD(list, result) \
	result = list; \
	while(result->prev != NULL) { \
		result = result->prev; \
	} \

#define GETLISTTAIL(list, result) \
	result = list; \
	while(result->next != NULL) { \
		result = result->next; \
	} \

void expressionReturn(expr_t*);
char* valueAsString(expr_t*);

symbol_t* curSymbol;

#endif


