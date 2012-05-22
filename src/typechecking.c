/*
 * @file typechecking.c
 *
 * http://uthash.sourceforge.net/
 *
 *  Created on: Mar 31, 2012
 *      Author: DirkK
 */

#include <uthash.h>
#include "symboltable.h"
#include "typechecking.h"
#include "generalParserFunc.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void typeError(int line, const char *msg, ...) {
	va_list fmtargs;
	char buffer[1024];
	va_start(fmtargs, msg);
	vsnprintf(buffer, sizeof(buffer) - 1, msg, fmtargs);
	va_end(fmtargs);
	fprintf(stderr, "line %d: %s\n", line, buffer);
}

//<<<<<<< HEAD
//
//int correctFuncTypesParam(int line, symbol_t* curSymbol, string funcID, param_t* parameters) {
//	func_t* function = findFunc(curSymbol, funcID);
//	param_t* parametersHash = function->param;
//	int i=1;
//	for(param_t* s=parametersHash; s != NULL; s=s->next) {
//		if(parameters==NULL) {
//			typeError(line, "Too few parameters; %d parameters expected, but %d parameters found",function->num_params,i);
//			return 0;
//		}
//		var_t* expression = (var_t*)parameters->var;
//		//fprintf(stderr, "%s", typeToString(expression->type));
//		if(s->var->type != expression->type) {
//			string strType = typeToString(s->var->type);
//			string strTypeExpr = typeToString(expression->type);
//			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",i,function->id,strType,strTypeExpr);
//			free((char*)strType);
//			free((char*)strTypeExpr);
//			return 0;
//		}
//		parameters = parameters->next;
//		i++;
//=======
/**
 * takes a list and a item of type paList, adds paList to the list
 * @param list
 * @param newItem
 */
paList_t* setToNextReference(paList_t* list, paList_t* newItem) {
	newItem->next = NULL;
	if (list != NULL) {
		newItem->prev = list;
		list->next = newItem;
	}
	list = newItem;
	return list;
}

paList_t* reverseParametersList(paList_t* paList) {
	while(paList->prev != NULL) {
		paList = paList->prev;
	}
	return paList;
}

/**
 * takes an expression or a parameter list, returning a list of strings containing the values/nums of all items
 * @param expressions
 * @param parameters
 * @param expressionList
 * @return
 */
paList_t* getValuesOfAllParameters(exprList_t* expressions, param_t* parameters,
		int expressionList) {
	paList_t* paraList = NULL;
	if (expressionList == 1) {
		for (exprList_t* s = expressions; s != NULL; s = s->next) {
			paList_t* paraListItem = malloc(sizeof(paList_t));
			if (paraListItem == NULL) {
				error("Could not allocate paraListItem");
			}
//			if (s->expr->valueKind == VAL_ID)
//				paraListItem->parameter = s->expr->value.id;
//			else if (s->expr->valueKind == VAL_NUM) {
//				char* temp = malloc(11);
//				sprintf(temp, "%d", s->expr->value.num);
//				paraListItem->parameter = temp;
//			}
			paraListItem->parameter = typeToString(s->expr->type);
			paraList = setToNextReference(paraList, paraListItem);
		}
	}
	if (expressionList == 0) {
		for (param_t* s = parameters; s != NULL; s = s->next) {
			paList_t* paraListItem = malloc(sizeof(paList_t));
			if (paraListItem == NULL) {
				error("Could not allocate paraListItem");
			}
			paraListItem->parameter = typeToString(s->var->type);
			paraList = setToNextReference(paraList, paraListItem);
		}
	}
	return reverseParametersList(paraList);
}

void printPaList(exprList_t* expressions, param_t* parameters,
		int isExpressionList) {
	if (isExpressionList == 0) {
		for (paList_t* s = getValuesOfAllParameters(NULL, parameters, 0);
				s != NULL; s = s->next) {
			fprintf(stderr, "%s, ", s->parameter);
		}
	} else if (isExpressionList == 1) {
		for (paList_t* s = getValuesOfAllParameters(expressions, NULL, 1);
				s != NULL; s = s->next) {
			fprintf(stderr, "%s, ", s->parameter);
		}
	}
}

void printExpectedFound(int line, param_t* parametersHash, exprList_t* expressions, param_t* parameters, int expressionList) {
	fprintf(stderr, "line %d: expected parameter types: ", line);
	printPaList(NULL, parametersHash, 0);
	fprintf(stderr, "\nline %d: found parameter types: ", line);
	if (expressionList == 0) {
		printPaList(NULL, parameters, 0);
	} else if (expressionList == 1) {
		printPaList(expressions, NULL, 1);
	}
	fprintf(stderr, "\n");
}

/**
 * checks for either a list of parameters or expressions if the types are correct
 * @param line
 * @param curSymbol
 * @param funcID
 * @param expressions
 * @param parameters
 * @param expressionList
 * @return 1 if correct / 0 if anything is nort correct, throws errors
 */
int correctFuncTypesReal(int line, symbol_t* curSymbol, string funcID,
		exprList_t* expressions, param_t* parameters, int isExpressionList) {
	exprList_t* expressionsOrg = expressions;
	param_t* parametersOrg = parameters;
	func_t* function = findFunc(curSymbol, funcID);
	param_t* parametersHash = function->param;
	int i = 1;
	for (param_t* s = parametersHash; s != NULL; s = s->next) {
		if ((isExpressionList == 0 && parameters == NULL)
				|| (isExpressionList == 1 && expressions == NULL)) {

			typeError(
					line,
					"Too few parameters; %d parameters expected, but %d parameters found",
					function->num_params, i - 1);
			printExpectedFound(line, parametersHash, expressionsOrg, parametersOrg, isExpressionList);
			return 0;
		}
		type_t paraExprType; // = malloc(sizeof(type_t));
		if (isExpressionList == 1) {
			paraExprType = expressions->expr->type;
		} else if (isExpressionList == 0) {
			paraExprType = parameters->var->type;
		}
		//fprintf(stderr, "%s", typeToString(expression->type));
		if(s->var->type != paraExprType) {
			string strType = typeToString(s->var->type);
			string strTypeExpr = typeToString(paraExprType);
			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",i,function->id,strType,strTypeExpr);
			printExpectedFound(line, parametersHash, expressionsOrg, parametersOrg, isExpressionList);
			free((char*)strType);
			free((char*)strTypeExpr);
			return 0;
		}
		if (isExpressionList == 1) {
			expressions = expressions->next;
		} else if (isExpressionList == 0) {
			parameters = parameters->next;
		}
		i++;
	}
	if ((isExpressionList == 0 && parameters != NULL)
			|| (isExpressionList == 1 && expressions != NULL)) {
		typeError(
				line,
				"Too much parameters; %d parameters expected, but %d parameters found",
				function->num_params, i);
		printExpectedFound(line, parametersHash, expressionsOrg, parametersOrg, isExpressionList);
		return 0;
	}

	return 1;
}

/**
 * calls correctFuncTypesReal for parameter list
 * @param line
 * @param curSymbol
 * @param funcID
 * @param parameters
 * @return
 */
int correctFuncTypesParam(int line, symbol_t* curSymbol, string funcID,
		param_t* parameters) {
	return correctFuncTypesReal(line, curSymbol, funcID, NULL, parameters, 0);
}

/**
 * calls correctFuncTypesReal for expression list
 * @param line
 * @param curSymbol
 * @param funcID
 * @param parameters
 * @return
 */
int correctFuncTypes(int line, symbol_t* curSymbol, string funcID,
		exprList_t* parameters) {
	return correctFuncTypesReal(line, curSymbol, funcID, parameters, NULL, 1);
}

/**
 * @brief TODO Dirk
 * @param line
 * @param type1
 * @param type2
 * @param isAssign
 * @return 0  if the types are not compatible
 * 1 if the types are compatible
 * if isAssign is set to 1 further checks are required:
 * 2: int array is assigned to an int
 */
int checkCompatibleTypesReal(int line, type_t type1, type_t type2, int isAssign) {
	if(isAssign == 1 && type1 == T_INT && type2 == T_INT_A) {
		return 2;
	} else if (type1 != type2) {
		typeError(line, "%s is incompatible with %s", typeToString(type1),
				typeToString(type2));
		return 0;
	}
	return 1;
}

// TODO Dirk document
int checkCompatibleTypesRaw(int line, type_t type1, type_t type2) {
	return checkCompatibleTypesReal(line, type1, type2, 0);
}

int checkCompatibleTypes(int line, expr_t* expr1, expr_t* expr2) {
	return checkCompatibleTypesReal(line, expr1->type, expr2->type, 0);
}

int checkCompatibleTypesAssign(int line, expr_t* expr1, expr_t* expr2) {
	return checkCompatibleTypesReal(line, expr1->type, expr2->type, 1);
}

int checkLValue(int line, expr_t* lvalue) {
	if(lvalue->lvalue == 0) {
		typeError(line, "it is not possible to assign a value to %s", valueAsString(lvalue));
		return 0;
	}
	return 1;
}

void checkReturnTypes(int line, type_t returnType, type_t returned) {
	if (returnType != returned) {
		typeError(line, "return type %s expected, but %s found", typeToString(returnType), typeToString(returned));
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

