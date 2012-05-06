/*
 * address_code.h
 *
 *  Created on: Mar 27, 2012
 *      Author: SebastianN
 */
#ifndef __ADDRESSCODE_HEADER__
#define __ADDRESSCODE_HEADER__

#include "types.h"
#include "symboltable.h"

struct irCode_t *irListTail;

int instruction;

void backpatch(indexList_t*, int);
indexList_t* merge(indexList_t*, indexList_t*);
indexList_t* createList(int);
expr_t* newTmp();
expr_t* newExpr(char*, type_t);

void printIRCode(FILE*, irCode_t*);
char* getConstOrId(expr_t*);
char* opToStr(operation_t);
/**
 * @brief Converts an IRCode into a string
 */
string IRtoString();
char* lineToString(char*, irCode_t*);
void printParam(irCode_t*);
/**
 * @brief Real conversion from expressions to IRCode
 */
void emit(expr_t*, expr_t*, operation_t, expr_t*);
irCode_t* getIRCode();

#endif
