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

/**
 * @brief symbol representation of a variable
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct var {
	char* id; // key
	type type;
	int size; // size that is needed (datatype and arraysize)
	int offset; // TODO
	UT_hash_handle hh; /* makes this structure hashable */
} var;

/**
 * @brief symbol representation of a function
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct func {
	char* id; // key
	type returnType;
	var* param; // hash table of variables, that are parameters
	UT_hash_handle hh; /* makes this structure hashable */
} func;

/**
 * @brief Symbol structure of the symboltable, implemented as a doubly linked list
 */
typedef struct symbol {
	var* symVar;
	func* symFunc;
	/*struct symbol* prev;*/ // this is actually not really needed, because we only have two different scope levels
	struct symbol* next; // with var names prev and next, this struct can be used with utlist
} symbol;

/**
 * @brief TODO
 */

typedef enum operations {
	OP_ASSIGN,
	OP_LOGICAL_OR,
	OP_LOGICAL_AND,
	OP_LOGICAL_NOT,
	OP_EQ,
	OP_NE,
	OP_LSEQ,
	OP_GTEQ,
	OP_GT,
	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_UNARY_MINUS
};

typedef struct irCode {
	int row;
	enum operations ops;
	var arg1;
	var arg2;
	var arg3;
	struct irCode *next; //Might be necessary.
} irCode;

#endif /* TYPES_H_ */
