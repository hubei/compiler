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

bool correctFuncTypes(string funcID, exprList* parameters) {
	func* function = findFunc(curSymbol, funcID);
	var* parametersHash = function->param;
	for(s=parametersHash; s != NULL; s=s->hh.next) {
		if(s.type != parameters->expr.type) {
			return false;
		}
		parameters = parameters->next;
	}
	return true;
}
