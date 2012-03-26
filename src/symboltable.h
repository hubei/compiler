/**
 * @file symboltable.c
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */
#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "include/uthash.h"
typedef const char* string;

/**
 * @brief set of possible types
 */
typedef enum type {
	T_INT,
	T_VOID,
	T_INT_A
} type;

/**
 * @brief symbol representation of a variable
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct var {
	char* id; // key
	type type;
	int size; // size that is needed (datatype and arraysize)
	UT_hash_handle hh; /* makes this structure hashable */
} var;

/**
 * @brief symbol representation of a function
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct func {
	char* id; // key
	type returnType;
	var* param; // hash table of variables, that are parameters
	UT_hash_handle hh; /* makes this structure hashable */
} func;

/**
 * @brief Symbol structure of the symboltable, implemented as a doubly linked list
 */
typedef struct symbol {
	var symVar;
	func symFunc;
	/*struct symbol* prev;*/ // this is actually not really needed, because we only have two different scope levels
	struct symbol* next; // with var names prev and next, this struct can be used with utlist
} symbol;


//##################################################################################################
symbol* push(symbol*); // save given symbol on stack and return new symbol with link to old
symbol* pop(symbol*); // return next symbol from stack

symbol* createSymbol();
var* createVar();
func* createFunc();
void addToVar(var* target, var* source);

void insertVar(symbol*, var*);
void insertFunc(symbol*, func*);
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
 * debug functions, only for testing
 * @param
 */
void debug(int);
void test_symTab();

#endif
