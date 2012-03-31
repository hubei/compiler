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
//symTab_t *symTab = NULL; /* NULL is important according to documentation */

/**
 * @brief Add a new entry to symTab, by giving the identifier (of function or variable)
 * The function will allocate the new entry and add it to the hash table.
 *
 * @param id Identifier of the function or variable
 * @return pointer to the symTabEntry, so that it can be filled
 */
//symTabEntry_t* addToSymTab(string id) {
//	symTab_t* s; /* must be of type symTab to be added to symTab */
//
//	s = findInSymTab(id); /* check if entry already exists */
//	if (!s) {
//		symTabEntry_t* entry = NULL; /* entry to be added to symTab */
//
////		entry = addToSymTabEntry(entry, IDENTIFIER, id);
//
//		s = malloc(sizeof(symTab_t));
//		s->id = malloc(sizeof(id));
//		strcpy(s->id, id);
//		s->entry = entry;
//
//		HASH_ADD_KEYPTR( hh, symTab, s->id, strlen(s->id), s);
//		return entry;
//	}
//	return s->entry;
//}
/**
 * @brief Add a new property (key/value pair) to the given entry table
 * If key already exists, the existing nothing will be saved
 * and the key/value pair will be returned
 *
 * @param entry An existing entry table where the new property should be stored
 * @param key A key, describing the property (use constants!)
 * @param value The value of the property
 */
//symTabEntry_t* addToSymTabEntry(symTabEntry_t* entry, int key, string value) {
//	symTabEntry_t *s; /* new property */
//
//	s = findInSymTabEntry(entry, key); /* check if entry already exists */
//	if (!s) {
//		s = malloc(sizeof(symTabEntry_t)); /* allocate space for new property */
//		s->key = key;
//		s->value = malloc(sizeof(value));
//		strcpy(s->value, value);
//
//		HASH_ADD_INT(entry, key, s);
//		/* add new property into the entry table */
//		return entry;
//	}
//	return s;
//}

/**
 * @brief Find given identifier in symbol table
 * @param id identifier to be found
 * @return symTab with id->entry pair
 */
//symTab_t* findInSymTab(string id) {
//	symTab_t* entry = NULL;
//	HASH_FIND_STR( symTab, id, entry);
//	return entry;
//}

/**
 * @brief Find given key in symbol table entry
 * @param s symbol table entry
 * @param key key to be found
 * @return symTabEntry with key->value pair
 */
//symTabEntry_t* findInSymTabEntry(symTabEntry_t* s, int key) {
//	symTabEntry_t* entry = NULL;
//	HASH_FIND_INT( s, &key, entry);
//	return entry;
//}

//void test_symTab() {
//	addToSymTab("test");
//	symTab_t* entry = findInSymTab("test");
//	symTabEntry_t* property = findInSymTabEntry(entry->entry, IDENTIFIER);
//	printf("\n%d -> %s\n", property->key, property->value);
//}

/**
 * @brief Only for debug: Print all identifiers in the symbol table
 */
//void printSymTabKeys() {
//	symTab_t *current_symTab, *tmp;
//	symTabEntry_t *current_Entry, *tmp2;
//
//	printf("\n");
//	HASH_ITER(hh, symTab, current_symTab, tmp) {
//		printf("%s\n", current_symTab->id);
//		HASH_ITER(hh,current_symTab->entry, current_Entry, tmp2) {
//			printf("  %10s -> %s\n", getKeyAsString(current_Entry->key), current_Entry->value);
//		}
//	}
//	printf("Total: %d", HASH_COUNT(symTab));
//}

/**
 * @brief Simple debug function that prints the given number followed by a '-'
 * @param number The number to be printed
 */
void debug(int number) {
	fprintf(stdout, "%d-", number);
}

//string getKeyAsString(int key) {
//	char* str;
//	switch(key) {
////	case TYPE: return "TYPE";
////	case IDENTIFIER: return "IDENTIFIER";
////	case RETURNTYPE: return "RETURNTYPE";
//	default:
//		str = malloc(3);
//		sprintf(str,"%d",key);
//		return str;
//	}
//}

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
		if(target == NULL) error("setString: Could not allocate new String target.");
		strcpy(target,source);
	} else {
		error("setString: Source is not initialized!");
	}
	return target;
}

symbol* push(symbol* symbol) {
	struct symbol* newSymbol = NULL;
	newSymbol = malloc(sizeof(struct symbol));
	if(newSymbol == NULL) error("push: Could not allocate new Symbol.");
	newSymbol->next = symbol;
	return newSymbol;
}

symbol* pop(symbol* symbol) {
	if(symbol == NULL) {
		error("pop: input is NULL");
	} else {
		if(symbol->next == NULL) {
			error("pop: failed to return next symbol: it is NULL");
		}
		return symbol->next;
	}
	return NULL;
}

//sicher so nich korrekt weil symbol nich die selbe struktur besitzt wie Var oder Func?

symbol* createSymbol() {
	symbol *newSymbol = NULL;
	newSymbol = malloc(sizeof(symbol));
		if(newSymbol==NULL){
			error("Symbol could not be created");
		}
		return newSymbol;
	return NULL;
}
/**
 * @brief
 *
 * @return
 */
var* createVar() {
	var *newVar = NULL;
	newVar = malloc(sizeof(var));
	if(newVar==NULL){
		error("Variable could not be created");
	}
	return newVar;
}
func* createFunc() {
	func *newFunc=NULL;
	newFunc=malloc (sizeof(func));
	if(newFunc==NULL){
		error("Function could not be created");
	}
	return newFunc;
}

/**
 *
 * @param target vorhandene hashtable
 * @param source einzelne Variable
 */
void addToVar(var* target, var* source){

}

//void insertVar(symbol* symbol, var* var){}
//void insertFunc(symbol* symbol, func* func){}
//var* findVar(symbol* symbol, string id){return NULL;} // find in current scope or scopes above
//func* findFunc(symbol* symbol, string id){return NULL;}
//int exists(symbol* symbol, string id){return 0;} // only in current scope

void insertVar(symbol* symbol, var var){
//prüfen ob übergebenen parameter nicht null sind
	if(symbol == NULL) error("---");
																//korrekt?
	if(&var == NULL) error("---");

	if(symbol->symVar == NULL){
		symbol->symVar=createVar();
	}
//einfügen in die hashmap
	HASH_ADD_KEYPTR( hh, symbol->symVar, var.id, strlen(var.id), &var);

}

// Test function var
void print_var(symbol* symbol) {
	if(symbol == NULL) error("---");

	struct var *k, *tmp;
	HASH_ITER(hh, symbol->symVar, k, tmp) {
        printf("var id %s: type %d: size %d: offset %d\n", k->id, k->type, k->size, k->offset);
    }
}

void insertFunc(symbol* symbol, func func){
	if(symbol == NULL) error("---");
																//korrekt?
	if(&func == NULL) error("---");

	if(symbol->symVar == NULL){
			symbol->symVar=createVar();
	}
//einfügen in die hashmap										//bei schreiben von func.id wird keine automische auswahl angegeben. besorgniserregend?
	HASH_ADD_KEYPTR( hh, symbol->symFunc, func.id, strlen(func.id), &func);
}

// Test function insertFunc
void print_func(symbol* symbol){
	struct func *k, *tmp;
	HASH_ITER(hh, symbol->symVar, k, tmp) {
	        printf("func id %s: type %d: ", k->id, k->returnType); 		//für param Hash map eigene function?
	}

}
// find in current scope or scopes above
//gibt das kein Return aus?
var* findVar(symbol* symbol, string id){
	struct var *k;
	HASH_FIND (hh,symbol->symVar,*id,strlen(var.id), k );
	RETURN k;														//sicher falsch, aber warum?
	//return NULL
}

func* findFunc(symbol* symbol, string id){
	struct func *k;
	HASH_FIND (hh,symbol->symFunc,*id,strlen(func.id), k );
	Return k;
	return NULL;
}

// etwas unsicher was zu tun. bzw wie man auf den scope beschränkt
int exists(symbol* symbol, string id){return 0;} // only in current scope

void error(string msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

