/*
 * symboltable.c
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */
#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include "include/uthash.h"
typedef const char* string;

typedef enum {
	T_INT,
	T_VOID,
	T_INT_A
} type;

typedef struct var {
	char* id;
	type type;
	int size;
	struct var* next;
} var;

typedef struct func {
	char* id;
	type returnType;
	var* param;
	struct func* next;
} func;

typedef struct symbol {
	var var;
	func func;
	struct symbol* superior;
} symbol;

/*
 * constants for identifying properties in the symbol table
 */
//#define IDENTIFIER 1 // function, variable or array identifier
//#define TYPE 2 // f: function, a: array, v: variable
//#define RETURNTYPE 3 // return type of function or type of variable/array
//#define ASIZE 4 // size of an array
//#define SCOPE 5 //

/**
 * @brief entry in the symbol table, that can store any property
 * the id should be accessed by constants, like TYPE
 */
typedef struct {
    int key; /* use constants to access properties */
	char* value;
    UT_hash_handle hh; /* makes this structure hashable */
} symTabEntry_t;

/**
 * @brief struct for storing the symbol table as a hash table
 * It maps a string identifier onto a symTabEntry that can store any information.
 * The identifier should be the name if function or variable
 */
typedef struct {
	char* id; /* name of function or variable */
    symTabEntry_t* entry;
    UT_hash_handle hh; /* makes this structure hashable */
} symTab_t;

void debug(int);
void test_symTab();

// Depreciated
symTabEntry_t* addToSymTab(string);
symTabEntry_t* addToSymTabEntry(symTabEntry_t*, int, string);
symTab_t* findInSymTab(string);
symTabEntry_t* findInSymTabEntry(symTabEntry_t*, int);

// new
void insertVar(string, int, int, int); //ID, type (constants!), arraysize (ignored, if not array), scopeID
void insertFunc(string, int); //ID, type
void addParam(string, string, int, int); // function ID, var ID, type, arraysize
int exist(string, int); //ID, scopeID
int getType(string, int); // ID, scope
int getScope(string); // ID; should return scope of function and create new scope, if it does not exists

// other
char* setString(const char*);
string getKeyAsString(int);

#endif
