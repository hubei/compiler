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

/** prototype needed for definition of func_t and symbol_t (they reference to each other) */
typedef struct symbol_t symbol_t;

/**
 * @brief string representation
 */
typedef const char* string;

/**
 * @brief set of possible data types
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
	/** a simple int index representing a reference to a line */
	int index;
	struct indexList_t* next;
	struct indexList_t* prev;
} indexList_t;

/**
 *	@brief postEmit is for functions and arrays: They have not enough information to emit themselves
 *	See also {@link: expr_t}
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
 * It holds data for type checking and IR code generation
 */
typedef struct expr_t {
	/** data type */
	type_t type;
	/** bool: whether the expression can stand on the left side of an assignment */
	int lvalue;
	/** bool: whether the expression is compound or raw */
	int compound;
	/** for array and function calls: emit later to decide if function is standalone or array is lvalue */
	postEmit_t postEmit;
	/** for goto statements to set jump location */
	int jump;
	/** to decide, if expression value is id or num */
	valueKind_t valueKind;
	/** for backpatching: collect all indices (rows) that need to be packpatched */
	indexList_t* trueList;
	/** for backpatching: collect all indices (rows) that need to be packpatched */
	indexList_t* falseList;
	/** list of expressions. Used for function call parameters in parser */
	exprList_t* params;
	/** name of variable or int value */
	union {
		char* id;
		int num;
	} value;
	/** for arrays: reference to expression that represents the indes */
	struct expr_t* arrInd;
	/** name of array/func in case, this is a tmpExpr that stores an arr/func value */
	char* parentId;
} expr_t;

/**
 *	@brief Possible operations, based on the expression-rules from the parser.y
 */
typedef enum operation_t {
	OP_ASSIGN, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_SHR, // shift right
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
	/** row/index of current line */
	int row;
	/** kind of address code operation */
	operation_t ops;
	/** result (depending on ops) */
	expr_t* res;
	/** first argument (depending on ops) */
	expr_t* arg0;
	/** second argument (depending on ops) */
	expr_t* arg1;
	struct irCode_t *prev, *next;
} irCode_t;

/**
 * @brief statement type for parser
 */
typedef struct stmt_t {
	/** return type of a function for typechecking */
	type_t returnType;
	/** for backpatching: list if indices */
	indexList_t* nextList;
} stmt_t;

/**
 * @brief symbol representation of a variable
 * This is a hash table. Its key is `id`, the rest are values.
 */
typedef struct var_t {
	/** name of variable - key of hash table */
	char* id; // key
	/** data type */
	type_t type;
	/** array size */
	int size;
	/** amount of space in memory */
	int width;
	/** memory location relative to scope */
	int offset;
	/** makes this structure hashable */
	UT_hash_handle hh;
} var_t;

/**
 * @brief Doubly LinkedList of vars for parameters of functions
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
	/** name of function - key of hash table */
	char* id; // key
	/** returned data type */
	type_t returnType;
	/** list of parameters */
	param_t* param;
	/** number of parameters for faster access */
	int num_params;
	/** reference to the corresponding scope of the function */
	symbol_t* symbol;
	/** makes this structure hashable */
	UT_hash_handle hh;
} func_t;

/**
 * @brief Symbol structure of the symbol table, implemented as a doubly linked list
 */
struct symbol_t {
	/** list of all containing variables */
	var_t* symVar;
	/** list of all containing functions. Does only apply to the global scope */
	func_t* symFunc;
	/** offset (next free memory location) of variable declaration (for new vars) */
	int offset;
	/** ircode of this function */
	irCode_t* ircode;
	/** overlying scope, should always be the global scope, as there are only two layers */
	struct symbol_t* next;
};

/**
 * @brief List of parameters represented as strings, only used for error messages
 * If a function is called with a wrong amount of types, you can print out all the types, which are expected and which are found
 */
typedef struct parameterList_t {
	/** type stored as string */
	string parameter;
	/** pointer to the previous type */
	struct parameterList_t* prev;
	/** pointer to the next type */
	struct parameterList_t* next;
} paList_t;

#endif /* TYPES_H_ */
