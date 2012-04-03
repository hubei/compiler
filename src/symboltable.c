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
 * @brief Simple debug function that prints the given number followed by a '-'
 * @param number The number to be printed
 */
void debug(int number) {
	fprintf(stdout, "%d-", number);
}


/**
 * @brief Take a string (like "this is a string"), allocate memory for it and return the address.
 *
 * @param source A string that should be stored in memory
 * @return A reference to the stored string
 */
char* setString(const char* source) {
	char* target;
	if (source != NULL) {
		target = malloc(sizeof(source));
		if (target == NULL)
			error("setString: Could not allocate new String target.");
		strcpy(target, source);
	} else {
		error("setString: Source is not initialized!");
	}
	return target;
}

symbol* push(symbol* symbol) {
	struct symbol* newSymbol = NULL;
	newSymbol = malloc(sizeof(struct symbol));
	if (newSymbol == NULL)
		error("push: Could not allocate new Symbol.");
	newSymbol->next = symbol;
	return newSymbol;
}

symbol* pop(symbol* symbol) {
	if (symbol == NULL) {
		error("pop: input is NULL");
	} else {
		if (symbol->next == NULL) {
			error("pop: failed to return next symbol: it is NULL");
		}
		return symbol->next;
	}
	return NULL;
}

//sicher so nich korrekt weil symbol nich die selbe struktur besitzt wie Var oder Func?
// -> sieht gut aus
symbol* createSymbol() {
	symbol *newSymbol = NULL;
	newSymbol = malloc(sizeof(symbol));
	if (newSymbol == NULL) {
		error("Symbol could not be created");
	}

	/* initialize all pointers to NULL
	 * This is very important, because C does not set a new pointer to NULL automatically
	 * This means, after declaration without initialization, the pointer can point anywhere!
	 *
	 * Also consider: To add e.g. a var to the hash table, the hash table can be NULL.
	 * It will be initialized correctly!
	 */
	newSymbol->symVar = NULL;
	newSymbol->symFunc = NULL;
	newSymbol->next = NULL;
	return newSymbol;
}

/**
 * @brief
 *
 * @return
 */
var* createVar(string id) {
	var *newVar = NULL;
	newVar = malloc(sizeof(var));
	if (newVar == NULL) {
		error("Variable could not be created");
	}
	newVar->id = malloc(sizeof(id));
	if(newVar->id == NULL) {
		error("createVar: Could not allocate id");
	}
	strcpy(newVar->id,id);
	return newVar;
}

/**
 *
 * @return
 */
func* createFunc(string id) {
	func *newFunc = NULL;
	newFunc = malloc(sizeof(func));
	if (newFunc == NULL) {
		error("Function could not be created");
	}
	newFunc->id = malloc(sizeof(id));
	if(newFunc->id == NULL) {
		error("createFunc: Could not allocate id");
	}
	strcpy(newFunc->id,id);
	return newFunc;
}

/**
 *
 * @param target vorhandene hashtable
 * @param source einzelne Variable
 */
void addToVar(var* target, var* source) {

}

/**
 *
 * @param symbol
 * @param var
 */
void insertVar(symbol* symbol, var var) {
	//prüfen ob übergebenen parameter nicht null sind
	if (symbol == NULL)
		error("---");
	if(var.id == NULL)
		error("insertVar: id of var is NULL!");
	//korrekt? -> nein, das ist nicht möglich ;) nur ein übergebener Pointer kann null sein
//	if (&var == NULL)
//		error("---");

//	if (symbol->symVar == NULL) {
//		symbol->symVar = createVar();
//	} -> siehe createsymbol
	//einfügen in die hashmap
	// symbol->symVar darf durchaus NULL sein.
	HASH_ADD_KEYPTR( hh, symbol->symVar, var.id, strlen(var.id), &var);

}

/**
 *
 * @param symbol
 * @param func
 */
void insertFunc(symbol* symbol, func func) {
	if (symbol == NULL)
		error("---");
	if(func.id == NULL)
		error("insertFunc: id of func is NULL!");
	//korrekt? -> siehe oben
//	if (&func == NULL)
//		error("---");

//	if (symbol->symVar == NULL) {
//		symbol->symVar = createVar();
//	} -> siehe createsymbol
	//einfügen in die hashmap
	//bei schreiben von func.id wird keine automische auswahl angegeben. besorgniserregend?
	// -> sieht doch gut aus
	struct func* test = malloc(sizeof(struct func));
	memcpy(test,&func,sizeof(struct func)); // irgendwas ist hier komisch... so gehts jedenfalls... bei var oben ging es mit &var
	HASH_ADD_KEYPTR( hh, symbol->symFunc, func.id, strlen(func.id), test);
}

/**
 * @brief Test function var
 * @param symbol
 */
void print_var(symbol* symbol) {
	if (symbol == NULL)
		error("---");

	struct var *k, *tmp;
	HASH_ITER(hh, symbol->symVar, k, tmp) {
		if(k->id == NULL)
			error("print_var: id is NULL!");
		printf("var id %s: type %d: size %d: offset %d\n", k->id, k->type,
				k->size, k->offset);
	}
}

/**
 * @brief Test function insertFunc
 * @param symbol
 */
void print_func(symbol* symbol) {
	if (symbol == NULL)
		error("---");

	struct func *k, *tmp;
	HASH_ITER(hh, symbol->symFunc, k, tmp) {
		if(k->id == NULL)
			error("print_func: id is NULL!");
		printf("func id %s: type %d: ", k->id, k->returnType); //für param Hash map eigene function?
	}

}
// find in current scope or scopes above
//gibt das kein Return aus?
// -> du suchst nur in symbol. Symbol hat den next-pointer, der auf das nächste Symbol zeigt.
// du musst diesem next Pointer folgende, bis dieser NULL ist. -> Schleife ;)
// gilt natürlich auch für findFunc
var* findVar(symbol* symbol, string id) {
	struct var *k;
	//HASH_FIND(hh, symbol->symVar, id, strlen(id), k);
	HASH_FIND_STR( symbol->symVar, id, k); // geht auch so, ist nicht so komplex
	return k; //sicher falsch, aber warum? -> ist korrekt, nur return muss komplett klein geschrieben werden ;)
}

/**
 *
 * @param symbol
 * @param id
 * @return
 */
func* findFunc(symbol* symbol, string id) {
	struct func *k;
	HASH_FIND(hh, symbol->symFunc, id, strlen(id), k);
	return k;
	return NULL;
}

// etwas unsicher was zu tun. bzw wie man auf den scope beschränkt
// -> du hast in den find Funktionen oben nur auf den aktuellen Scope beschränkt.
// D.h.: exists wie oben, aber einmal in symFunc und einmal in symVar suchen
int exists(symbol* symbol, string id) {
	return 0;
} // only in current scope

/**
 * @brief Output given message and exit program
 * @param message
 */
void error(string msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void test_symTab(symbol* symbol) {
	print_var(symbol);
}

