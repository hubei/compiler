/*
 * symboltable.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 6, 2012
 *      Author: NicolaiO
 */

#include "symboltable.h"
#include <uthash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct symbol_t* symbolTable = NULL;

/**
 * @brief Take a string (like "this is a string"), allocate memory for it and return the address.
 * @param source A string that should be stored in memory
 * @return A reference to the stored string
 */
char* setString(char* source) {
	char* target = NULL;
	if (source != NULL) {
		target = malloc(strlen(source) + 1);
		if (target == NULL) {
			// TODO error
		}
		strcpy(target, source);
	} else {
		error("setString: Source is not initialized!");
	}
	return target;
}

/**
 * @brief Output given message and exit program
 * @param msg message to be printed to stderr
 */
void error(string msg) {
	// TODO error
	fprintf(stderr, "\n%s\nExiting...\n", msg);
	exit(1);
}

/**
 * @brief return a new symbol that represents the scope of func
 * @param symbol current symbol
 * @param func function that the new scope is linked to
 * @return new symbol
 */
symbol_t* push(symbol_t* symbol, struct func_t* func) {
	assert(func!=NULL);

	if (func->symbol == NULL) {
		func->symbol = createSymbol();
		func->symbol->next = symbol;
	}
	return func->symbol;
}

/**
 * @brief leave given symbol (scope) by returning the associated symbol above
 * should always return the global scope symbol...
 * @param symbol current symbol
 * @return associated symbol (normally global)
 */
symbol_t* pop(symbol_t* symbol) {
	assert(symbol!=NULL);
	assert(symbol->next!=NULL);
	return symbol->next;
}

/**
 * @brief create a new empty symbol
 * @return new symbol, is a struct, containing hash tables for variables and functions
 */
symbol_t* createSymbol() {
	symbol_t *newSymbol = NULL;
	newSymbol = malloc(sizeof(symbol_t));
	if (newSymbol == NULL) {
		// TODO error
		error("createSymbol: Could not allocate newSymbol");
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
	newSymbol->offset = 0;
	return newSymbol;
}

/**
 * @brief creates a new Variable
 * @param id, identifies a var
 * @return
 */
var_t* createVar(string id) {
	assert(id != NULL);

	var_t *newVar = NULL;
	newVar = malloc(sizeof(var_t));
	if (newVar == NULL) {
		// TODO error
		error("Variable could not be created");
	}
	newVar->id = malloc(strlen(id) + 1);
	if (newVar->id == NULL) {
		// TODO error
		error("createVar: Could not allocate id");
	}
	strcpy(newVar->id, id);

	newVar->offset = 0;
	newVar->size = 0;
	newVar->type = T_UNKNOWN;
	newVar->width = 0;
	return newVar;
}

/**
 * @brief creates a new Function.
 * @param id, identifies a function
 * @return
 */
func_t* createFunc(string id) {
	assert(id != NULL);

	func_t *newFunc = NULL;
	newFunc = malloc(sizeof(struct func_t));
	if (newFunc == NULL) {
		// TODO error
		error("Function could not be created");
	}
	newFunc->id = malloc(strlen(id) + 1);
	if (newFunc->id == NULL) {
		// TODO error
		error("createFunc: Could not allocate id");
	}
	strcpy(newFunc->id, id);

	newFunc->num_params = 0;
	newFunc->param = NULL;
	newFunc->returnType = T_UNKNOWN;
	newFunc->symbol = NULL;
	return newFunc;
}

/**
 * @brief create a new param from given paramVar and add it to the end of other params
 * @param prevParam end of list of all previouse params
 * @param paramVar var object that represents the param
 * @return reference to new param
 */
param_t* addParam(param_t* prevParam, var_t* paramVar) {
	assert(paramVar!=NULL);
	assert(paramVar->id!=NULL);

	struct param_t* newParam = NULL;
	newParam = malloc(sizeof(var_t));
	if (newParam == NULL) {
		// TODO error
	}

	// initialize
	newParam->prev = NULL;
	newParam->next = NULL;
	newParam->var = paramVar;

	// Concatenate with prevParam
	if (prevParam != NULL) {
		prevParam->next = newParam;
		newParam->prev = prevParam;
	}

	return newParam;
}

/**
 * @brief adds an variable to an existing symbol
 * @param symbol, is a struct, containing hash tables for variables and functions
 * @param var, hash table, defining variables
 */
void insertVar(symbol_t* symbol, var_t* var) {
	assert(symbol != NULL);
	assert(var != NULL);
	assert(var->id != NULL);

	if (exists(symbol, var->id)) {
		// TODO error
		error("Function is already in symbol table");
	}

	// offset
	var->offset = symbol->offset;
	symbol->offset += var->width;

	// insert into hashmap
	// symbol->symVar can be NULL! Thats not problem
	HASH_ADD_KEYPTR( hh, symbol->symVar, var->id, strlen(var->id), var);
}

/**
 * @brief remove given var from symbolTable and delete it (free)
 * @param id name of var
 */
void destroyVar(symbol_t* curSymbol, string id) {
	assert(id != NULL);
	var_t* var = findVar(curSymbol, id);
	if(var != NULL) {
		HASH_DEL(curSymbol->symVar, var);
		free(var);
	}
}

/**
 * @brief adds a function to an existing symbol
 * @param symbol, symbol, is a struct, containing hash tables for variables and functions
 * @param func, hash table, defining functions
 */
void insertFunc(symbol_t* symbol, func_t* func) {
	assert(symbol != NULL);
	assert(func != NULL);
	assert(func->id != NULL);

	if (exists(symbol, func->id)) {
		// TODO error
		error("Function is already symbol table");
	}

	//einfügen in die hashmap
	HASH_ADD_KEYPTR( hh, symbol->symFunc, func->id, strlen(func->id), func);
}

/**
 * @brief insert the full list of params to the given function
 * this function will also set the offset
 * it should be called after inserting the function, but before function body!
 * @param func function to insert params into
 * @param lastParam reference to the last param of the paramList
 */
void insertParams(func_t* func, param_t* lastParam) {
	assert(func!=NULL);
	assert(func->symbol != NULL);
	param_t* param = lastParam;
	var_t* var = NULL;
	int paramCount = 0;
	while (param != NULL) {
		var = param->var;
		assert(var!=NULL);

		// offset
		var->offset = func->symbol->offset;
		func->symbol->offset += var->width;

		HASH_ADD_KEYPTR( hh, func->symbol->symVar, var->id, strlen(var->id),
				var);
		func->param = param;
		param = param->prev;
		paramCount++;
	}

	func->num_params = paramCount;
}

/**
 * @brief return the number of parameter contained in given param list
 * @param paramHead first param of paramList
 * @return number of params
 */
int getParamCount(param_t* paramHead) {
	int count = 0;
	param_t* param = paramHead;
	while (param != NULL) {
		count++;
		param = param->next;
	}
	return count;
}

/**
 * @brief sarches for variables in all scopes of the symbol table
 * @param symbol
 * @param id
 * @return the variable
 */
var_t* findVar(symbol_t* symbol, string id) {
	assert(symbol != NULL);
	assert(id != NULL);
	struct var_t *k;
	struct symbol_t* tmpSymbol = symbol;
	while (tmpSymbol != NULL) {
		HASH_FIND(hh, tmpSymbol->symVar, id, strlen(id), k);
		if (k != NULL) {
			return k;
		}
		tmpSymbol = tmpSymbol->next;
	}
	return NULL;
}

/**
 * @brief searches for functions in all scopes  of the symbol table
 * @param symbol
 * @param id
 * @return the function
 */
func_t* findFunc(symbol_t* symbol, string id) {
	assert(symbol != NULL);
	assert(id != NULL);
	struct func_t *k = NULL;
	struct symbol_t* tmpSymbol = symbol;
	while (tmpSymbol != NULL) {
		HASH_FIND(hh, tmpSymbol->symFunc, id, strlen(id), k);
		if (k != NULL) {
			return k;
		}
		tmpSymbol = tmpSymbol->next;
	}
	return NULL;
}

/**
 * @brief a function which checks, whether a function or variable
 * exist in a scope or not
 * @param symbol
 * @param id
 * @return 1 if id exists, 0 if not
 */
int exists(symbol_t* symbol, string id) {
	assert(symbol != NULL);
	assert(id != NULL);
	struct func_t *k = NULL;
	HASH_FIND(hh, symbol->symFunc, id, strlen(id), k);
	if (k != NULL) {
		return 1;
	}
	HASH_FIND(hh, symbol->symVar, id, strlen(id), k);
	if (k != NULL) {
		return 1;
	}
	return 0;
}

/**
 * @brief Get the symbol pointing to the global scope
 * @return symbol to global scope
 */
symbol_t* getSymbolTable() {
	return symbolTable;
}

/**
 * @brief Set symbol table (this will be the global symbol)
 * @param sym new symbol table
 */
void setSymbolTable(symbol_t* sym) {
	symbolTable = sym;
}

/**
 * @brief Print all given variables
 * @param file an open FILE or NULL for stdout
 * @param symVar hash table of variables
 * @param kind should be either "var" or "param"
 */
void print_var(FILE* file, var_t* symVar) {
	assert(file!=NULL);
	// symVar can be NULL -> empty hash table

	struct var_t *k = NULL, *tmp = NULL;
	HASH_ITER(hh, symVar, k, tmp) {
		assert(k->id != NULL);
		string strType = typeToString(k->type);
		fprintf(file,
				"var %s\n\ttype: %s - size: %d - width: %d - offset: %d\n",
				k->id, strType, k->size, k->width, k->offset);
		free((char*)strType);
	}
}

/**
 * @brief prints parameters of variables
 * @param file specifies the location of the file where the parameters are printed to
 * @param paramHead
 */
void print_param(FILE* file, param_t* paramHead) {
	assert(file!=NULL);
	// paramHead can be NULL -> no params

	param_t* param = paramHead;
	var_t* var = NULL;
	while (param != NULL) {
		var = param->var;
		assert(var!=NULL);
		string strType = typeToString(var->type);
		fprintf(file,
				"\tparam %s\n\t\ttype: %s - size: %d - width: %d - offset: %d\n",
				var->id, strType, var->size, var->width,
				var->offset);
		free((char*)strType);
		param = param->next;
	}
}

/**
 * @brief print all given functions and its parameters
 * @param file an open FILE or NULL for stdout
 * @param symFunc hash table of functions
 */
void print_func(FILE* file, func_t* symFunc) {
	assert(file!=NULL);
	// symFunc can be NULL -> empty hash table

	struct func_t *func, *tmp;
	HASH_ITER(hh, symFunc, func, tmp) {
		assert(func->id != NULL);
		string strType = typeToString(func->returnType);
		fprintf(file, "func %s\n\treturntype: %s - num_params: %d\n", func->id,
				strType, func->num_params);
		free((char*)strType);

		// print params, if there are any
		print_param(file, func->param);
	}
}

/**
 * @brief print symTab into given file or to stdout if file == NULL
 * @param file link to an open file or NULL
 */
void print_symTab(FILE* file) {
	// use stdout, if there is no file
	if (file == NULL) {
		file = stdout;
	}

	// print global
	fprintf(file, "################### global scope ###################\n");
	print_var(file, symbolTable->symVar);
	print_func(file, symbolTable->symFunc);

	// print symbol table for each func
	struct func_t *func, *tmp;
	HASH_ITER(hh, symbolTable->symFunc, func, tmp) {
		if (func->symbol != NULL) {
			fprintf(file,
					"################### scope for %s ###################\n",
					func->id);
			print_var(file, func->symbol->symVar);
			print_func(file, func->symbol->symFunc);
		}
	}
}

/**
 *
 * @param type
 * @return
 */
string typeToString(type_t type) {
	switch (type) {
	case T_INT:
		return setString("INT");
		break;
	case T_INT_A:
		return setString("INT array");
		break;
	case T_VOID:
		return setString("VOID");
	case T_UNKNOWN:
		return setString("UNKOWN");
		break;
	}
	return setString("no valid type");
}

