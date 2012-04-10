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
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


void typeError (int line, const char *msg, ...)
{
	va_list fmtargs;
	char buffer[1024];
	va_start(fmtargs,msg);
	vsnprintf(buffer,sizeof(buffer)-1,msg,fmtargs);
	va_end(fmtargs);
	fprintf(stderr, "line %d: %s\n",line,buffer);
}



void correctFuncTypes(int line, symbol* curSymbol, string funcID, exprList* parameters) {
	func* function = findFunc(curSymbol, funcID);
	param* parametersHash = function->param;
	int i=1;
	for(param* s=parametersHash; s != NULL; s=s->next) {
		if(parameters==NULL) {
			typeError(line, "Too few parameters; %d parameters expected, but %d parameters found",function->num_params,i);
			return;
		}
		expr* expression = (expr*)parameters->expr;
		//fprintf(stderr, "%s", typeToString(expression->type));
		if(s->var->type != expression->type) {
			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",i,function->id,typeToString(s->var->type),typeToString(expression->type));
		}
		parameters = parameters->next;
		i++;
	}
}

void checkCompatibleTypes(int line, expr expr1, expr expr2){
	if(expr1.type != expr2.type) {
		typeError(line, "%s is incompatible with %s", typeToString(expr1.type), typeToString(expr2.type));
	}
}

//void checkCompatibleTypes(int line, expr expr1, expr expr2){
//	checkCompatibleTypesInfo(line, expr1, expr2, "");
//}

//int correctReturnType(symbol* curSymbol, string funcID, expr* returnTypeExpr) {
//	func* function = findFunc(curSymbol, funcID);
//	if(function->returnType == returnTypeExpr->type) {
//		return 1;
//	}
//	return 0;
//}



