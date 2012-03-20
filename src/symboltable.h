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

/*
 * constants for identifying properties in the symbol table
 */
#define IDENTIFIER 1 // function, variable or array identifier
#define TYPE 2 // f: function, a: array, v: variable
#define RETURNTYPE 3 // return type of function or type of variable/array
#define ASIZE 4 // size of an array
#define SCOPE 5 //

// types
#define INT 6
#define VOID 7

// var
#define VAR 8

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
symTabEntry_t* addToSymTab(string);
symTabEntry_t* addToSymTabEntry(symTabEntry_t*, int, string);
symTab_t* findInSymTab(string);
symTabEntry_t* findInSymTabEntry(symTabEntry_t*, int);
string getKeyAsString(int);
char* setString(const char*);

void insertVar(string, int, int); //ID, type(0 int, 1 void), scopeID
void insertFunc(string, int); //ID, type ( INT, VOID )
void addParam(string, string, int); // function ID, var ID, type
int exist(string, int); //ID, scopeID
int getType(string, int); // ID, scope

#endif
