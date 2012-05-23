/**
 * @file types.h
 * @brief Contains all global types that are used within the compiler
 *
 *  Created on: Mar 31, 2012
 *      Author: NicolaiO
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <uthash.h>
#include <utlist.h>
typedef struct symbol_t symbol_t; // prototype need for definition

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
 * @brief possible kinds of values
 */
typedef enum valueKind_t {
	VAL_UNKOWN, VAL_ID, VAL_NUM
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
 *	@brief postEmit is for functions and arrays: They have not enough information to emit themselves
 */
typedef enum postEmit_t {
	PE_NONE, PE_ARR, PE_FUNCC
} postEmit_t;

/**
 * @brief expression list of the parser (function parameters)
 */
typedef struct exprList_t {
	struct expr_t* expr;
	struct exprList_t* prev;
	struct exprList_t* next;
} exprList_t;

/**
 * @brief Expressions are used to transfer information within the parser
 */
typedef struct expr_t {
	type_t type;
	int lvalue;
	postEmit_t* postEmit; // for array and function calls: emit later to decide if function is standalone or array is lvalue
	int jump; // for goto statements to set jump location
	valueKind_t valueKind;
	indexList_t* trueList;
	indexList_t* falseList;
	exprList_t* params;
	union {
		char* id;
		int num;
	} value;
	// for arrays:
	struct expr_t* arrInd; // expression for array index
	char* parentId; // name of array/func in case, this is a tmpExpr that stores an arr/func value
} expr_t;

/**
 * @brief statement type for parser
 */
typedef struct stmt_t {
	type_t returnType;
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

/**
 * @brief LinkedList of vars for parameters of functions
 */
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
 *	@brief Based on the expression-rules from the parser.y
 */
typedef enum operation_t {
	OP_ASSIGN,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_SHR, // shift right
	OP_SHL, // shift left
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

/**
 * 	@brief One row of a IR 3-address code with references to next and previous line
 */
typedef struct irCode_t {
	int row;
	operation_t ops;
	expr_t* res;
	expr_t* arg0;
	expr_t* arg1;
	struct irCode_t *prev, *next;
} irCode_t;

#endif /* TYPES_H_ */
