/**
 * @file typechecking.h
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */
#ifndef TYPECHECKING_H_
#define TYPECHECKING_H_

#include "uthash.h"

typedef struct parameterList_t {
	string parameter;
	struct parameterList_t* prev;
	struct parameterList_t* next;
} paList_t;

//int correctReturnType(symbol*, string funcID, expr returnTypeExpr);
int checkCompatibleTypes(int line, expr_t* expr1, expr_t* expr2);
int checkCompatibleTypesRaw(int line, type_t type1, type_t type2);
//void checkCompatibleTypesInfo(int line, expr expr1, expr expr2, char* type);
int correctFuncTypes(int line, symbol_t*, string funcID, exprList_t* parameters);
int correctFuncTypesParam(int line, symbol_t*, string funcID, param_t* parameters);
void typeError (int line, const char *msg, ...);

#endif
