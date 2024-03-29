/**
 * @file typechecking.h
 * @brief Contains functions for type checking
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */
#ifndef TYPECHECKING_H_
#define TYPECHECKING_H_

#include <uthash.h>




int checkCompatibleTypes(int line, expr_t* expr1, expr_t* expr2);
int checkCompatibleTypesAssign(int line, expr_t* expr1, expr_t* expr2);
int checkCompatibleTypesRaw(int line, type_t type1, type_t type2);

void checkReturnTypes(int line, type_t returnType, type_t returned);

int correctFuncTypes(int line, symbol_t*, string funcID, exprList_t* parameters);
int correctFuncTypesParam(int line, symbol_t*, string funcID, param_t* parameters);

int checkLValue(int line, expr_t* lvalue);

void typeError (int line, const char *msg, ...);

void clean_paList(paList_t* paList);

#endif
