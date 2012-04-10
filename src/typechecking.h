/**
 * @file symboltable.c
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */
#ifndef TYPECHECKING_H_
#define TYPECHECKING_H_

#include "include/uthash.h"
#include "include/utlist.h"


//int correctReturnType(symbol*, string funcID, expr returnTypeExpr);
int checkCompatibleTypes(int line, expr expr1, expr expr2);
//void checkCompatibleTypesInfo(int line, expr expr1, expr expr2, char* type);
int correctFuncTypes(int line, symbol*, string funcID, exprList* parameters);
void typeError (int line, const char *msg, ...);

#endif
