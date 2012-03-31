/**
 * @file symboltable.c
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */
#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "types.h"



//##################################################################################################
symbol* push(symbol*); // save given symbol on stack and return new symbol with link to old
symbol* pop(symbol*); // return next symbol from stack

symbol* createSymbol();
var* createVar();
func* createFunc();
void addToVar(var* target, var* source);

void insertVar(symbol*, var);
void insertFunc(symbol*, func);
var* findVar(symbol*, string); // find in current scope or scopes above
func* findFunc(symbol*, string);
int exists(symbol*, string); // only in current scope
//##################################################################################################
//void insertVar(symbol*, string, type, int); // symTab (for scope), ID, type , size (1 for vars)
//void insertFunc(symbol*, string, type); //ID, returntype
//void addParam(string, string, int, int); // function ID, var ID, type, arraysize
//int exist(string, int); //ID, scopeID
//int getType(string, int); // ID, scope
//int getScope(string); // ID; should return scope of function and create new scope, if it does not exists

/**
 * @brief Allocate space for the given string and return address
 * @param string to be copied
 * @return pointer to new string
 */
char* setString(const char*);

/**
 * @brief Output given message and exit program
 * @param message
 */
void error(string);

/**
 * debug functions, only for testing
 * @param
 */
void debug(int);
void test_symTab();

#endif
