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

struct symbol* symbolTable = NULL;


/**
 * @brief Simple debug function that prints the given number followed by a '-'
 * @param number The number to be printed
 */
void debug(int number) {
//	fprintf(stdout, "%d-", number);
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

/**
 *
 * @param symbol
 * @return
 */
symbol* push(symbol* symbol, struct func* func) {
	if(func == NULL)
		error("push: func is NULL");
	if(func->symbol == NULL) {
		func->symbol = createSymbol();
		func->symbol->next = symbol;
	}
	return func->symbol;
}

/**
 *
 * @param symbol
 * @return
 */
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

/**
 * @brief
 * @return
 */
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
 *
 * @param id
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
	newFunc->num_params = 0;
	return newFunc;
}

/**
 *
 * @param hash vorhandene hashtable
 * @param newVar einzelne Variable
 */
var* addParamToParamhash(var* hash, var* newVar) {
	if(newVar==NULL)
		error("---");

	if(newVar->id==NULL)
		error("---");

	// does not work, no idea of how to check that hash is a hash table and not only a var
//	if(hash != NULL && hash->hh == NULL)
//		error("addParamToParamhash: non empty hash table has no hash handler");

	HASH_ADD_KEYPTR( hh, hash, newVar->id, strlen(newVar->id), newVar);
	return hash;
}

/**
 *
 * @param symbol
 * @param var
 */
void insertVar(symbol* symbol, var* var) {
	//prüfen ob übergebenen parameter nicht null sind
	if (symbol == NULL)
		error("---");
	if(var == NULL)
		error("---");
	if(var->id == NULL)
		error("insertVar: id of var is NULL!");

	// TODO Dirk check for existing

	//einfügen in die hashmap
	// symbol->symVar darf durchaus NULL sein.
	HASH_ADD_KEYPTR( hh, symbol->symVar, var->id, strlen(var->id), var);
}

/**
 *
 * @param symbol
 * @param func
 */
void insertFunc(symbol* symbol, func* func) {
	if (symbol == NULL)
		error("---");
	if (func == NULL)
		error("---");
	if(func->id == NULL)
		error("insertFunc: id of func is NULL!");

	// TODO Dirk check for existing

	//einfügen in die hashmap
	HASH_ADD_KEYPTR( hh, symbol->symFunc, func->id, strlen(func->id), func);
}

/**
 * @brief insert the full hash map of params for the given function
 * @param func
 * @param params
 */
void insertParams(func* func, var* params) {
	func->param = params;
	func->num_params = HASH_COUNT(params);
}

/**
 * @brief Insert given parameters into symbol table
 * @param symbol the symbol where to insert params
 * @param params params (from a function) to be inserted
 */
void addParamsToSymbol(symbol* symbol, var* params) {
	var *var, *tmp2;
	HASH_ITER(hh, params, var, tmp2) {
		insertVar(symbol, var);
	}
}

/**
 *
 * @param symbol
 * @param id
 * @return
 */
var* findVar(symbol* symbol, string id) {
	struct var *k;
	HASH_FIND(hh, symbol->symVar, id, strlen(id), k);
	return k;
}

/**
 * @brief searches for functions in all scopes
 * @param symbol
 * @param id
 * @return the function
 */
func* findFunc(symbol* symbol, string id) {
	struct func *k=NULL;
	struct symbol* tmpSymbol = symbol;
	while(symbol!=NULL){
		HASH_FIND(hh, symbol->symFunc, id, strlen(id), k);
		if(k!=NULL){
			return k;
		}
		tmpSymbol = symbol->next;
	}
	k=NULL;
	return k;
}

/**
 * @brief a function which proves, whether an function or variable
 * exist in a scope or not
 * @param symbol
 * @param id
 * @return 1 if id exists, 0 if not
 */
int exists(symbol* symbol, string id) {
	struct func *k;
	HASH_FIND(hh, symbol->symFunc, id, strlen(id), k);
	if(k!=NULL){return 1;};
	HASH_FIND(hh, symbol->symVar, id, strlen(id), k);
	if(k!=NULL){return 1;};
	return 0;
}

/**
 * @brief Get the symbol pointing to the global scope
 * @return symbol to global scope
 */
symbol* getSymbolTable() {
	return symbolTable;
}

/**
 * @brief Set symbol table
 * @param sym new symbol table
 */
void setSymbolTable(symbol* sym) {
	symbolTable = sym;
}

/**
 * @brief Output given message and exit program
 * @param msg
 */
void error(string msg) {
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

/**
 * @brief Test function var
 * @param symbol
 */
void print_var(FILE* file, symbol* symbol) {
	if (symbol == NULL)
		error("-2-");

	struct var *k, *tmp;
	HASH_ITER(hh, symbol->symVar, k, tmp) {
		if(k->id == NULL)
			error("print_var: id is NULL!");
		fprintf(file, "var %s\n\ttype: %s - size %d - offset %d\n", k->id, typeToString(k->type),
				k->size, k->offset);
	}
}

/**
 * @brief Test function insertFunc
 * @param symbol
 */
void print_func(FILE* file, symbol* symbol) {
	if (symbol == NULL)
		error("-1-");

	struct func *func, *tmp;
	struct var *var, *tmp2;
	HASH_ITER(hh, symbol->symFunc, func, tmp) {
		if(func->id == NULL)
			error("print_func: id is NULL!");
		fprintf(file, "func %s\n\treturntype: %s - num_params: %d\n", func->id, typeToString(func->returnType),func->num_params);

		// print params, if there are any
		if(func->param != NULL) {
			HASH_ITER(hh, func->param, var, tmp2) {
				if(var->id == NULL)
					error("print_func: param id is NULL!");
				fprintf(file, "\tparam %s\n\t\ttype: %s - size %d - offset %d\n", var->id, typeToString(var->type),
						var->size, var->offset);
			}
		}
	}
}

/**
 * @brief print symTab into given file or to stdout, if file == NULL
 * @param file link to open file or NULL
 */
void test_symTab(FILE* file) {
	if(file == NULL) {
		file = stdout;
	}

	// print global
	fprintf(file,"################### global scope ###################\n");
	print_var(file,symbolTable);
	print_func(file,symbolTable);

	// print symbol table for each func
	struct func *func, *tmp;
	HASH_ITER(hh, symbolTable->symFunc, func, tmp) {
		if(func->symbol != NULL) {
			fprintf(file,"################### scope for %s ###################\n",func->id);
			print_var(file,func->symbol);
			print_func(file,func->symbol);
		}
	}
}

/**
 *
 * @param type
 * @return
 */
string typeToString(type type) {
	switch(type) {
	case T_INT: return setString("INT"); break;
	case T_INT_A: return setString("INT array"); break;
	case T_VOID: return setString("VOID"); break;
	}
	return setString("unknown");
}

