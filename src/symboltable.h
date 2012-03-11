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
#define IDENTIFIER 1
#define TYPE 2

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

#endif
