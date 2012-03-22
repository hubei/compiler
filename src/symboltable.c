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

	s = findInSymTab(id); /* check if entry already exists */
	if (!s) {
		symTabEntry_t* entry = NULL; /* entry to be added to symTab */

		entry = addToSymTabEntry(entry, IDENTIFIER, id);

		s = malloc(sizeof(symTab_t));
		s->id = malloc(sizeof(id));
		strcpy(s->id, id);
		s->entry = entry;

		HASH_ADD_KEYPTR( hh, symTab, s->id, strlen(s->id), s);
		return entry;
	}
	return s->entry;
}

/**
 * @brief Add a new property (key/value pair) to the given entry table
 * If key already exists, the existing nothing will be saved
 * and the key/value pair will be returned
 *
 * @param entry An existing entry table where the new property should be stored
 * @param key A key, describing the property (use constants!)
 * @param value The value of the property
 */
symTabEntry_t* addToSymTabEntry(symTabEntry_t* entry, int key, string value) {
	symTabEntry_t *s; /* new property */

	s = findInSymTabEntry(entry, key); /* check if entry already exists */
	if (!s) {
		s = malloc(sizeof(symTabEntry_t)); /* allocate space for new property */
		s->key = key;
		s->value = malloc(sizeof(value));
		strcpy(s->value, value);

		HASH_ADD_INT(entry, key, s);
		/* add new property into the entry table */
		return entry;
	}
	return s;
}

/**
 * @brief Find given identifier in symbol table
 * @param id identifier to be found
 * @return symTab with id->entry pair
 */
symTab_t* findInSymTab(string id) {
	symTab_t* entry = NULL;
	HASH_FIND_STR( symTab, id, entry);
	return entry;
}

/**
 * @brief Find given key in symbol table entry
 * @param s symbol table entry
 * @param key key to be found
 * @return symTabEntry with key->value pair
 */
symTabEntry_t* findInSymTabEntry(symTabEntry_t* s, int key) {
	symTabEntry_t* entry = NULL;
	HASH_FIND_INT( s, &key, entry);
	return entry;
}

void test_symTab() {
	addToSymTab("test");
	symTab_t* entry = findInSymTab("test");
	symTabEntry_t* property = findInSymTabEntry(entry->entry, IDENTIFIER);
	printf("\n%d -> %s\n", property->key, property->value);
}

/**
 * @brief Only for debug: Print all identifiers in the symbol table
 */
void printSymTabKeys() {
	symTab_t *current_symTab, *tmp;
	symTabEntry_t *current_Entry, *tmp2;

	printf("\n");
	HASH_ITER(hh, symTab, current_symTab, tmp) {
		printf("%s\n", current_symTab->id);
		HASH_ITER(hh,current_symTab->entry, current_Entry, tmp2) {
			printf("  %10s -> %s\n", getKeyAsString(current_Entry->key), current_Entry->value);
		}
	}
	printf("Total: %d", HASH_COUNT(symTab));
}

/**
 * @brief Simple debug function that prints the given number followed by a '-'
 * @param number The number to be printed
 */
void debug(int number) {
	fprintf(stdout, "%d-", number);
}

string getKeyAsString(int key) {
	char* str;
	switch(key) {
	case TYPE: return "TYPE";
	case IDENTIFIER: return "IDENTIFIER";
	case RETURNTYPE: return "RETURNTYPE";
	default:
		str = malloc(3);
		sprintf(str,"%d",key);
		return str;
	}
}

/**
 * @brief Take a string (like "this is a string"), allocate memory for it and return the address.
 *
 * @param source A string that should be stored in memory
 * @return A reference to the stored string
 */
char* setString(const char* source) {
	char* target;
	if(source) {
		target = malloc(sizeof(source));
		strcpy(target,source);
	} else {
		fprintf(stderr,"setString: Source is not initialized!");
	}
	return target;
}

void insertVar(string id, int type, int arraysize, int scope) {

}

void insertFunc(string id, int type) {

}

int getScope(string id) {

	return 0;
}
