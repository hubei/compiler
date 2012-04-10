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
 * @brief Output given message and exit program
 * @param msg message to be printed to stderr
 */
void error(string msg) {
	fprintf(stderr, "\n%s\n", msg);
	exit(1);
}

/**
 * @brief return a new symbol that represents the scope of func
 * @param symbol current symbol
 * @param func function that the new scope is linked to
 * @return new symbol
 */
symbol* push(symbol* symbol, struct func* func) {
	if (func == NULL)
		error("push: func is NULL");
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
 * @brief create a new empty symbol
 * @return new symbol
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
	newSymbol->offset = 0;
	return newSymbol;
}

/**
 * @brief
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
	if (newVar->id == NULL) {
		error("createVar: Could not allocate id");
	}
	strcpy(newVar->id, id);
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
	if (newFunc->id == NULL) {
		error("createFunc: Could not allocate id");
	}
	strcpy(newFunc->id, id);
	newFunc->num_params = 0;
	return newFunc;
}

/**
 * @brief create a new param from given paramVar and at it to the end of other params
 * @param prevParam end of list of all previouse params
 * @param paramVar var object that represents the param
 * @return reference to new param
 */
param* addParam(param* prevParam, var* paramVar) {
	if(paramVar==NULL)
		error("---");

	if(paramVar->id==NULL)
		error("---");

	struct param* newParam = NULL;
	newParam = malloc(sizeof(paramVar));
	if(newParam == NULL)
		error("addParam: newParam is NULL");

	newParam->prev = NULL;
	newParam->next = NULL;
	newParam->var = paramVar;

	if(prevParam != NULL) {
		prevParam->next = newParam;
		newParam->prev = prevParam;
	}

	return newParam;
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
	if (var == NULL)
		error("---");
	if (var->id == NULL)
		error("insertVar: id of var is NULL!");

	// TODO Dirk check for existing

	// offset
	var->offset = symbol->offset;
	symbol->offset += var->width;

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
	if (func->id == NULL)
		error("insertFunc: id of func is NULL!");

	// TODO Dirk check for existing

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
void insertParams(func* func, param* lastParam) {
	if(func == NULL)
		error("insertParams: func is NULL!");
	if(func->symbol == NULL)
		error("insertParams: func->symbol is NULL");

	param* param = lastParam;
	var* var = NULL;
	int paramCount = 0;
	while(param != NULL) {
		var = param->var;
		if(var == NULL)
			error("insertParams: var is NULL");

		// offset
		var->offset = func->symbol->offset;
		func->symbol->offset += var->width;

		HASH_ADD_KEYPTR( hh, func->symbol->symVar, var->id, strlen(var->id), var);
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
int getParamCount(param* paramHead) {
	int count = 0;
	param* param = paramHead;
	while(param != NULL) {
		count++;
		param = param->next;
	}
	return count;
}

/**
 *
 * @param symbol
 * @param id
 * @return
 */
var* findVar(symbol* symbol, string id) {
	struct var *k;
	struct symbol* tmpSymbol = symbol;
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
 * @brief searches for functions in all scopes
 * @param symbol
 * @param id
 * @return the function
 */
func* findFunc(symbol* symbol, string id) {
	struct func *k = NULL;
	struct symbol* tmpSymbol = symbol;
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
int exists(symbol* symbol, string id) {
	struct func *k = NULL;
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
symbol* getSymbolTable() {
	return symbolTable;
}

/**
 * @brief Set symbol table (this will be the global symbol)
 * @param sym new symbol table
 */
void setSymbolTable(symbol* sym) {
	symbolTable = sym;
}

/**
 * @brief Print all given variables
 * @param file an open FILE or NULL for stdout
 * @param symVar hash table of variables
 * @param kind should be either "var" or "param"
 */
void print_var(FILE* file, var* symVar) {
	if (symVar == NULL)
		return;

	struct var *k, *tmp;
	HASH_ITER(hh, symVar, k, tmp) {
		if (k->id == NULL)
			error("print_var: id is NULL!");
		fprintf(file, "var %s\n\ttype: %s - size: %d - width: %d - offset: %d\n", k->id,
				typeToString(k->type), k->size, k->width, k->offset);
	}
}

void print_param(FILE* file, param* paramHead) {
//	if (symVar == NULL)
//		return;

	param* param = paramHead;
	var* var = NULL;
	while(param != NULL) {
		var = param->var;
		if(var == NULL)
			error("print_param: var is NULL");
		fprintf(file, "\tparam %s\n\t\ttype: %s - size: %d - width: %d - offset: %d\n", var->id,
						typeToString(var->type), var->size, var->width, var->offset);
		param = param->next;
	}

//	struct var *k, *tmp;
//	HASH_ITER(hh, symVar, k, tmp) {
//		if (k->id == NULL)
//			error("print_var: id is NULL!");
//		fprintf(file, "%s %s\n\ttype: %s - size: %d - width: %d - offset: %d\n", kind, k->id,
//				typeToString(k->type), k->size, k->width, k->offset);
//	}
}

/**
 * @brief print all given functions and its parameters
 * @param file an open FILE or NULL for stdout
 * @param symFunc hash table of functions
 */
void print_func(FILE* file, func* symFunc) {
	if (symFunc == NULL)
		return;

	struct func *func, *tmp;
	HASH_ITER(hh, symFunc, func, tmp) {
		if (func->id == NULL)
			error("print_func: id is NULL!");
		fprintf(file, "func %s\n\treturntype: %s - num_params: %d\n", func->id,
				typeToString(func->returnType), func->num_params);

		// print params, if there are any
		print_param(file,func->param);
	}
}

/**
 * @brief print symTab into given file or to stdout if file == NULL
 * @param file link to an open file or NULL
 */
void test_symTab(FILE* file) {
	if (file == NULL) {
		file = stdout;
	}

	// print global
	fprintf(file, "################### global scope ###################\n");
	print_var(file, symbolTable->symVar);
	print_func(file, symbolTable->symFunc);

	// print symbol table for each func
	struct func *func, *tmp;
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
string typeToString(type type) {
	switch (type) {
	case T_INT:
		return setString("INT");
		break;
	case T_INT_A:
		return setString("INT array");
		break;
	case T_VOID:
		return setString("VOID");
		break;
	}
	return setString("unknown");
}

