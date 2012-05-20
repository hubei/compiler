/**
 * @file address_code.h
 * @brief Contains functions for generating the intermediate code
 *
 *  Created on: Mar 27, 2012
 *      Author: SebastianN
 */
#ifndef __ADDRESSCODE_HEADER__
#define __ADDRESSCODE_HEADER__

#include "types.h"
#include "symboltable.h"

irCode_t* irListTail;
int instruction;
int nextTmpVar;

// general irCode functions
void backpatch(indexList_t*, int);
void emit(expr_t*, expr_t*, operation_t, expr_t*);
indexList_t* merge(indexList_t*, indexList_t*);

// creation functions
indexList_t* newIndexList(int);
expr_t* newTmp(type_t);
expr_t* newExpr(char*, type_t);
expr_t* newAnonymousExpr();
expr_t* newExprNum(int, type_t);
stmt_t* newStmt();

// delete functions
void delLastInstr();

// debug functions for printing, etc.
void printIRCode(FILE*, irCode_t*);
char* opToStr(operation_t);
char* exprListToStr(exprList_t*);

// interface for getting complete irCode at the end
irCode_t* getIRCode();

#endif
