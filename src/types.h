/*
 * types.h
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "uthash.h"
#include "utlist.h"
typedef struct symbol_t symbol_t; // prototype

/**
 * @brief string representation
 */
typedef const char* string;

/**
 * @brief set of possible types
 */
typedef enum type_t {
	T_UNKNOWN, T_INT, T_VOID, T_INT_A
} type_t;

/**
 * TODO Dirk documentation
 */
typedef enum valueKind_t {
	VAL_ID, VAL_NUM
} valueKind_t;

/**
 * @brief list of indices
 */
typedef struct indexList_t {
	int index;
	struct indexList_t* next;
	struct indexList_t* prev;
} indexList_t;

/**
 * TODO Dirk documentation
 */
typedef struct expr_t {
	type_t type;
	int lvalue;
	int jump; // for goto statements to set jump location
	valueKind_t valueKind;
	indexList_t* trueList;
	indexList_t* falseList;
	union {
		char* id;
		int num;
	} value;
} expr_t;

/**
 * @brief expression list of the parser (function parameters)
 */
typedef struct exprList_t {
	struct expr_t* expr;
	struct exprList_t* prev;
	struct exprList_t* next;
} exprList_t;

/**
 * @brief statement type for parser
 */
typedef struct stmt_t {
	indexList_t* nextList;
} stmt_t;

/**
 * @brief symbol representation of a variable
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct var_t {
	char* id; // key
	type_t type;
	int size; // array size
	int width; // amount of space in memory
	int offset; // memory location relative to scope
	UT_hash_handle hh; /* makes this structure hashable */
} var_t;

typedef struct param_t {
	var_t* var;
	struct param_t* next;
	struct param_t* prev;
} param_t;

/**
 * @brief symbol representation of a function
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct func_t {
	char* id; // key
	type_t returnType;
	param_t* param; // hash table of variables, that are parameters
	int num_params;
	symbol_t* symbol;
	UT_hash_handle hh; /* makes this structure hashable */
} func_t;

/**
 * @brief Symbol structure of the symbol table, implemented as a doubly linked list
 */
struct symbol_t {
	var_t* symVar;
	func_t* symFunc;
	int offset; // offset of variable declaration (for new vars)
	struct symbol_t* next; // overlying scope, should always be the global scope, as there are only two layers
};

/**
 *	Based on the expression-rules from the parser.y
 */
typedef enum operation_t {
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
} operation_t;

///**
// *  @brief	Enables the determination whether a type is a Variable, Function or constant
// */
//typedef enum irType_t {
//	ARG_UNKOWN, ARG_VAR, ARG_FUNC, ARG_CONST
//} irType_t;
//
///**
// *  @brief	IrCode argument. Can be either a variable, function or constant.
// * 	In order to determine which one is stored, a type will be assigned, based on the
// * 	previous enum.
// */
//
//typedef struct irCode_arg_t {
//	union {
//		var_t* _var;
//		func_t* _func;
//		int _constant;
//	} arg;
//	irType_t type;
//} irCode_arg_t;

/**
 * 	@brief One row of a IR 3-address code with references to next and previous line
 */
typedef struct irCode_t {
	int row;
	char* label; // optional label FIXME not sure if needed
	operation_t ops;
	expr_t* res;
	expr_t* arg0;
	expr_t* arg1;
	struct irCode_t *prev, *next;
} irCode_t;

#endif /* TYPES_H_ */
