/*
 * symboltable.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */

#include "include/uthash.h"
#include "symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief symbol table object that stores the actual symbol table
 */
symTab_t *symTab = NULL; /* NULL is important according to documentation */

/**
 * @brief Add a new entry to symTab, by giving the identifier (of function or variable)
 * The function will allocate the new entry and add it to the hash table.
 *
 * @param id Identifier of the function or variable
 * @return pointer to the symTabEntry, so that it can be filled
 */
symTabEntry_t* addToSymTab(string id) {
	symTab_t* s; /* must be of type symTab to be added to symTab */
	symTabEntry_t* entry = NULL; /* entry to be added to symTab */

	entry = addToSymTabEntry(entry,IDENTIFIER, id);

	s = malloc(sizeof(symTab_t));
	s->id = malloc(sizeof(id));
	strcpy(s->id, id);
	s->entry = entry;

	HASH_ADD_KEYPTR( hh, symTab, s->id, strlen(s->id), s );
    return entry;
}

/**
 * @brief Add a new property (key/value pair) to the given entry table
 * @param entry An existing entry table where the new property should be stored
 * @param key A key, describing the property (use constants!)
 * @param value The value of the property
 */
symTabEntry_t* addToSymTabEntry(symTabEntry_t* entry, int key, string value) {
	symTabEntry_t *s; /* new property */

	s = malloc(sizeof(symTabEntry_t)); /* allocate space for new property */
	s->key = key;
	s->value = malloc(sizeof(value));
	strcpy(s->value,value);

	HASH_ADD_INT(entry,key,s); /* add new property into the entry table */
	return entry;
}

symTab_t* findInSymTab(string id) {
	symTab_t* entry = NULL;

    HASH_FIND_STR( symTab, id, entry );  /* s: output pointer */
    return entry;
}

symTabEntry_t* findInSymTabEntry(symTabEntry_t* s, int id) {
	symTabEntry_t* entry = NULL;

    HASH_FIND_INT( s, &id, entry );  /* s: output pointer */
    return entry;
}

void test_symTab() {
	addToSymTab("test");
	symTab_t* entry = findInSymTab("test");
	symTabEntry_t* property = findInSymTabEntry(entry->entry, IDENTIFIER);
	printf("\n%d -> %s\n", property->key,property->value);
}

void printSymTabKeys() {
	symTab_t* current, *tmp;

	printf("\n");
	HASH_ITER(hh, symTab, current, tmp) {
		printf("%s\n",current->id);
	}
	printf("Total: %d",HASH_COUNT(symTab));
}

/**
 * @brief Simple debug function that prints the given number followed by a '-'
 * @param number The number to be printed
 */
void debug(int number) {
	fprintf(stdout,"%d-", number);
}

