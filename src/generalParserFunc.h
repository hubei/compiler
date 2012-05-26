/**
 * @file generalParserFunc.h
 * @brief Contains some functions that are used in different modules
 *
 *  Created on: Apr 10, 2012
 *      Author: DirkK
 */
#ifndef GENERALPARSERFUNC_H_
#define GENERALPARSERFUNC_H_

/** @brief current symbol in parser */
symbol_t* curSymbol;

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

/**
 * @brief Same as GETLISTHEAD, but for tail
 */
#define GETLISTTAIL(list, result) \
	result = list; \
	while(result->next != NULL) { \
		result = result->next; \
	} \


char* valueAsString(expr_t*);
void clean_stmt(stmt_t* stmt);
void clean_indexList(indexList_t* il);

#endif


