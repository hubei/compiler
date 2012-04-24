/*
 * @file typechecking.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include "uthash.h"
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


int correctFuncTypesParam(int line, symbol_t* curSymbol, string funcID, param_t* parameters) {
	func_t* function = findFunc(curSymbol, funcID);
	param_t* parametersHash = function->param;
	int i=1;
	for(param_t* s=parametersHash; s != NULL; s=s->next) {
		if(parameters==NULL) {
			typeError(line, "Too few parameters; %d parameters expected, but %d parameters found",function->num_params,i);
			return 0;
		}
		var_t* expression = (var_t*)parameters->var;
		//fprintf(stderr, "%s", typeToString(expression->type));
		if(s->var->type != expression->type) {
			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",i,function->id,typeToString(s->var->type),typeToString(expression->type));
			return 0;
		}
		parameters = parameters->next;
		i++;
	}
	return 1;
}

int correctFuncTypes(int line, symbol_t* curSymbol, string funcID, exprList_t* parameters) {
	func_t* function = findFunc(curSymbol, funcID);
	param_t* parametersHash = function->param;
	int i=1;
	for(param_t* s=parametersHash; s != NULL; s=s->next) {
		if(parameters==NULL) {
			typeError(line, "Too few parameters; %d parameters expected, but %d parameters found",function->num_params,i);
			return 0;
		}
		expr_t* expression = (expr_t*)parameters->expr;
		//fprintf(stderr, "%s", typeToString(expression->type));
		if(s->var->type != expression->type) {
			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",i,function->id,typeToString(s->var->type),typeToString(expression->type));
			return 0;
		}
		parameters = parameters->next;
		i++;
	}
	if(parameters != NULL) {
		typeError(line, "Too much parameters; %d parameters expected, but %d parameters found",function->num_params,i);
		return 0;
	}

	return 1;
}

int checkCompatibleTypesRaw(int line, type_t type1, type_t type2){
	if(type1 != type2) {
		typeError(line, "%s is incompatible with %s", typeToString(type1), typeToString(type2));
		return 0;
	}
	return 1;
}

int checkCompatibleTypes(int line, expr_t* expr1, expr_t* expr2){
	return checkCompatibleTypesRaw(line, expr1->type, expr2->type);
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



