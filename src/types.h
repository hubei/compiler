/*
 * types.h
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "include/uthash.h"
#include "include/utlist.h"
typedef struct symbol symbol; // prototype

/**
 * @brief string representation
 */
typedef const char* string;

/**
 * @brief set of possible types
 */
typedef enum type {
	T_INT, T_VOID, T_INT_A
} type;
typedef enum valueKind {
		VAL_ID, VAL_NUM
}valueKind;



typedef struct {
	type type;
	int lvalue;
	valueKind valueKind;
	union{
		char* id;
		int num;
	}value;
} expr;

/**
 * @brief expression list of the parser (function parameters)
 */
typedef struct exprList {
		struct expr* expr;
		struct exprList* prev;
		struct exprList* next;
} exprList;



/**
 * @brief symbol representation of a variable
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct var {
	char* id; // key
	type type;
	int size; // array size
	int width; // amount of space in memory
	int offset; // memory location relative to scope
	UT_hash_handle hh; /* makes this structure hashable */
} var;

typedef struct param {
	var* var;
	struct param* next;
	struct param* prev;
} param;

/**
 * @brief symbol representation of a function
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct func {
	char* id; // key
	type returnType;
	param* param; // hash table of variables, that are parameters
	int num_params;
	symbol* symbol;
	UT_hash_handle hh; /* makes this structure hashable */
} func;

/**
 * @brief Symbol structure of the symbol table, implemented as a doubly linked list
 */
struct symbol {
	var* symVar;
	func* symFunc;
	int offset; // offset of variable declaration (for new vars)
	struct symbol* next; // overlying scope, should always be the global scope, as there are only two layers
};

/**
 *	Based on the expression-rules from the parser.y
 */
typedef enum operations {
	OP_ASSIGN,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_MINUS,
	OP_IFEQ,
	OP_IFNE,
	OP_IFGT,
	OP_IFGE,
	OP_IFLT,
	OP_IFLE,
	OP_GOTO,
	OP_RETURN_VAL,
	OP_RETURN_VOID,
	OP_CALL_RES,
	OP_CALL_VOID,
	OP_ARRAY_LD,
	OP_ARRAY_ST
} operations;

/**
 *  @brief	Enables the determination whether a type is a Variable, Function or constant
 */
typedef enum irType {
	ARG_VAR,
	ARG_FUNC,
	ARG_CONST
} irType ;

/**
 *  @brief	IrCode argument. Can be either a variable, function or constant.
 * 	In order to determine which one is stored, a type will be assigned, based on the
 * 	previous enum.
 */

typedef struct irCode_arg {
	union {
		var* _var;
		func* _func;
		int _constant;
	} arg;
    int type;
} irCode_arg;

/**
 * 	@brief Actual IR 3-address code. It always contains a pointer to the next expression, until ultimately
 * 	an "=" is read or a function is called.
 */
typedef struct irCode {
	int row;
	int ops;
	irCode_arg res;
	irCode_arg arg0;
	irCode_arg arg1;
	struct irCode *next; //Next operation until NULL
} irCode;

#endif /* TYPES_H_ */
