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
var* createVar(string);
func* createFunc(string);
void addToVar(var* target, var* source);

void insertVar(symbol*, var);
void insertFunc(symbol*, func);
var* findVar(symbol*, string); // find in current scope or scopes above
func* findFunc(symbol*, string);
int exists(symbol*, string); // only in current scope

char* setString(const char*);
void error(string);
void debug(int);
void test_symTab(symbol*);

#endif
