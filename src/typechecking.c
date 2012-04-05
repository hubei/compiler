/*
 * symboltable.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "include/uthash.h"
#include "symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int correctFuncTypes(symbol* curSymbol, string funcID, exprList* parameters) {
	func* function = findFunc(curSymbol, funcID);
	var* parametersHash = function->param;
	int i=1;
	for(var* s=parametersHash; s != NULL; s=s->hh.next) {
		expr* expression = (expr*)parameters->expr;
		if(s->type != expression->type) {
			return i;
		}
		parameters = parameters->next;
		i++;
	}
	return 0;
}

int correctReturnType(symbol* curSymbol, string funcID, expr* returnTypeExpr) {
	func* function = findFunc(curSymbol, funcID);
	if(function->returnType == returnTypeExpr->type) {
		return 1;
	}
	return 0;
}

