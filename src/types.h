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


/*
 *	Based on the expression-rules from the parser.y
 *	TODO Basti hier die richtigen code aus der aufg beschr
 */
typedef enum operations {
	OP_ASSIGN,     //!< OP_ASSIGN
	OP_LOGICAL_OR, //!< OP_LOGICAL_OR
	OP_LOGICAL_AND,//!< OP_LOGICAL_AND
	OP_LOGICAL_NOT,//!< OP_LOGICAL_NOT
	OP_EQ,         //!< OP_EQ
	OP_NE,         //!< OP_NE
	OP_LSEQ,       //!< OP_LSEQ
	OP_GTEQ,       //!< OP_GTEQ
	OP_GT,         //!< OP_GT
	OP_PLUS,       //!< OP_PLUS
	OP_MINUS,      //!< OP_MINUS
	OP_MUL,        //!< OP_MUL
	OP_DIV,        //!< OP_DIV
	OP_MOD,        //!< OP_MOD
	OP_UNARY_MINUS //!< OP_UNARY_MINUS
};

/**
 * 	@brief Enables the determination whether a type is a Variable, Function or constant
 */
typedef enum irType {
	ARG_VAR,
	ARG_FUNC,
	ARG_CONST
} ;

/*
 * 	IrCode argument. Can be either a variable, function or constant.
 * 	In order to determine which one is stored, a type will be assigned, based on the
 * 	previous enum.
 */
typedef struct irCode_arg {
	union {
		var* _var;
		func* _func;
		int _constant;
	} arg;
	enum irType type;
} irCode_arg;

/*
 * 	Actual IR 3-address code. It always contains a pointer to the next expression, until ultimately
 * 	an "=" is read or a function is called.
 */
typedef struct irCode {
	int row;
	enum operations ops;
	irCode_arg arg0;
	irCode_arg arg1;
	irCode_arg arg2;
	struct irCode *next; //Next operation until NULL
} irCode;

#endif /* TYPES_H_ */
