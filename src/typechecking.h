/**
 * @file symboltable.c
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */
#ifndef TYPECHECKING_H_
#define TYPECHECKING_H_

#include "include/uthash.h"
#include "include/utlist.h"


int correctFuncTypes(string funcID, exprList* parameters);

#endif
