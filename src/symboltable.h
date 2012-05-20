/**
 * @file symboltable.h
 * @brief Contains functions for generating, accessing and printing the symbol table
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */
#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "types.h"
#include <stdio.h> // FILE

/**
 * @brief pointer to the top of the symbol table
 */
struct symbol_t* symbolTable;

symbol_t* push(symbol_t*, func_t*); // save given symbol on stack and return new symbol with link to old
symbol_t* pop(symbol_t*); // return next symbol from stack

symbol_t* createSymbol();
var_t* createVar(string);
func_t* createFunc(string);

param_t* addParam(param_t*,var_t*);
void insertParams(func_t*,param_t*);
int getParamCount(param_t*);

void insertVar(symbol_t*, var_t*);
void insertFunc(symbol_t*, func_t*);
var_t* findVar(symbol_t*, string); // find in current scope or scopes above
func_t* findFunc(symbol_t*, string);
int exists(symbol_t*, string); // only in current scope
void destroyVar(symbol_t*, string);

char* setString(char*);
string typeToString(type_t);
void error(string);

void print_param(FILE*,param_t*);
void print_var(FILE*,var_t*);
void print_func(FILE*,func_t*);
void print_symTab(FILE*);

symbol_t* getSymbolTable();
void setSymbolTable(symbol_t*);

#endif
