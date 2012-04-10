/**
 * @file symboltable.c
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */
#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "types.h"
#include "stdio.h"

#define GETLISTHEAD(list, result) \
	result = list; \
	while(result->prev != NULL) { \
		result = result->prev; \
	} \

/**
 * @brief pointer to the top of the symbol table
 */
struct symbol* symbolTable;

symbol* push(symbol*, func*); // save given symbol on stack and return new symbol with link to old
symbol* pop(symbol*); // return next symbol from stack

symbol* createSymbol();
var* createVar(string);
func* createFunc(string);

param* addParam(param*,var*);
void insertParams(func*,param*);
int getParamCount(param*);

void insertVar(symbol*, var*);
void insertFunc(symbol*, func*);
var* findVar(symbol*, string); // find in current scope or scopes above
func* findFunc(symbol*, string);
int exists(symbol*, string); // only in current scope

char* setString(const char*);
string typeToString(type);
void error(string);
void debug(int);

void print_param(FILE*,param*);
void print_var(FILE*,var*);
void print_func(FILE*,func*);
void test_symTab(FILE*);

symbol* getSymbolTable();
void setSymbolTable(symbol*);

#endif
