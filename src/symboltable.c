/*
 * symboltable.c
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */

#include "include/uthash.h"
#include "symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef const char* string;

struct symTabEntry {
	string key;
	string value;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct symTab {
    string id;            /* we'll use this field as the key */
    struct symTabEntry entry;
    UT_hash_handle hh; /* makes this structure hashable */
};

struct symTab *symTab = NULL;

void add_function(struct symTab *s) {
    HASH_ADD_STR( symTab, id, s );
}

UT_hash_handle symTable;

void debug(int key) {
	fprintf(stdout,"%d-", key);
}

