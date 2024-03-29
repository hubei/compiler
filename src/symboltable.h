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

void errorFuncDeclared(int line, string id);
void errorVarDeclared(int line, string id);
void errorIdDeclared(int line, string id);

void print_param(FILE*,param_t*);
void print_var(FILE*,var_t*);
void print_func(FILE*,func_t*);
void print_symTab(FILE*);

symbol_t* getSymbolTable();
void setSymbolTable(symbol_t*);

void clean_func(func_t* func);
void clean_var(var_t* var);
void clean_symbol(symbol_t* symbol);
void clean_varList(var_t* varList);
void clean_funcList(func_t* funcList);
void clean_paramList(param_t* paramList);
void clean_paramList2(param_t* paramList, int rmVars);
void clean_ircode(irCode_t* ircode);

#endif
