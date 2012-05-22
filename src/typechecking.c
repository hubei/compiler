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

/**
 * error function for type checking
 * @param line
 * @param msg error message
 * @param ... list of parameters used in the string
 */
void typeError(int line, const char *msg, ...) {
	va_list fmtargs;
	char buffer[1024];
	va_start(fmtargs, msg);
	vsnprintf(buffer, sizeof(buffer) - 1, msg, fmtargs);
	va_end(fmtargs);
	fprintf(stderr, "line %d: %s\n", line, buffer);
}

/**
 * @brief takes a list and an item of type paList, adds paList to the list
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

/**
 * @brief sets the pointer of a paList to the first item
 * @param paList
 * @return pointer to the first item
 */
paList_t* reverseParametersList(paList_t* paList) {
	while(paList->prev != NULL) {
		paList = paList->prev;
	}
	return paList;
}

/**
 * takes an expression or a parameter list, returning a list of strings containing the types of all items
 * @param expressions list of expressions
 * @param parameters list of parameters
 * @param isExpressionList 1 = expressions should be used, 0 = parameters should be used
 * @return a type list
 */
paList_t* getTypesOfAllParameters(exprList_t* expressions, param_t* parameters,
		int isExpressionList) {
	paList_t* paraList = NULL;
	if (isExpressionList == 1) {
		for (exprList_t* s = expressions; s != NULL; s = s->next) {
			paList_t* paraListItem = malloc(sizeof(paList_t));
			if (paraListItem == NULL) {
				error("Could not allocate paraListItem");
			}
			paraListItem->parameter = typeToString(s->expr->type);
			paraList = setToNextReference(paraList, paraListItem);
		}
	}
	if (isExpressionList == 0) {
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

/**
 * prints the types of a list of expressions or parameters divided by ,
 * @param expressions
 * @param parameters
 * @param isExpressionList 1 = expressions should be used, 0 = parameters should be used
 */
void printPaList(exprList_t* expressions, param_t* parameters,
		int isExpressionList) {
	for (paList_t* s = getTypesOfAllParameters(expressions, parameters, isExpressionList);
			s != NULL; s = s->next) {
		fprintf(stderr, "%s, ", s->parameter);
	}
}

/**
 * @brief prints out as error the expected and the found expression / parameter types
 * @param line
 * @param expectedParams list of expected parameters
 * @param expressions list of found expressions
 * @param parameters list of found parameters
 * @param isExpressionList 1 = expressions should be used, 0 = parameters should be used
 */
void printExpectedFound(int line, param_t* expectedParams, exprList_t* expressions, param_t* parameters, int isExpressionList) {
	fprintf(stderr, "line %d: expected parameter types: ", line);
	printPaList(NULL, expectedParams, 0);
	fprintf(stderr, "\nline %d: found parameter types: ", line);
	if (isExpressionList == 0) {
		printPaList(NULL, parameters, 0);
	} else if (isExpressionList == 1) {
		printPaList(expressions, NULL, 1);
	}
	fprintf(stderr, "\n");
}

/**
 * checks for either a list of parameters or expressions if the types are correct
 * @param line
 * @param curSymbol important to find the function out of the funcID
 * @param funcID the ID of the called function as string
 * @param expressions list of found expressions
 * @param parameters list of found parameters
 * @param isExpressionList 1 = expressions should be used, 0 = parameters should be used
 * @return 1 if correct / 0 if anything is not correct, throws errors
 */
int correctFuncTypesReal(int line, symbol_t* curSymbol, string funcID,
		exprList_t* expressions, param_t* parameters, int isExpressionList) {

	//saves the original lists (for output purpose)
	exprList_t* expressionsOrg = expressions;
	param_t* parametersOrg = parameters;

	func_t* function = findFunc(curSymbol, funcID);
	param_t* parametersHash = function->param;
	int count = 1;
	for (param_t* s = parametersHash; s != NULL; s = s->next) {

		//check if there are too few parameters (found list is at the end, but expected list not)
		if ((isExpressionList == 0 && parameters == NULL)
				|| (isExpressionList == 1 && expressions == NULL)) {
			typeError(
					line,
					"Too few parameters; %d parameters expected, but %d parameters found",
					function->num_params, count - 1);
			printExpectedFound(line, parametersHash, expressionsOrg, parametersOrg, isExpressionList);
			return 0;
		}

		//point paraExprType to the type of expression / parameter
		type_t paraExprType;
		if (isExpressionList == 1) {
			paraExprType = expressions->expr->type;
		} else if (isExpressionList == 0) {
			paraExprType = parameters->var->type;
		}

		//check if the found and expected types are the same, if not throws an error
		if(s->var->type != paraExprType) {
			string strType = typeToString(s->var->type);
			string strTypeExpr = typeToString(paraExprType);
			typeError(line, "Type of parameter %d is incompatible in function call %s; %s expected, but %s found",count,function->id,strType,strTypeExpr);
			printExpectedFound(line, parametersHash, expressionsOrg, parametersOrg, isExpressionList);
			free((char*)strType);
			free((char*)strTypeExpr);
			return 0;
		}

		//iterate over the found parameters
		if (isExpressionList == 1) {
			expressions = expressions->next;
		} else if (isExpressionList == 0) {
			parameters = parameters->next;
		}
		count++;
	}

	//check if there are too many parameters (expected list is at the end, but found list not)
	if ((isExpressionList == 0 && parameters != NULL)
			|| (isExpressionList == 1 && expressions != NULL)) {
		typeError(
				line,
				"Too many parameters; %d parameters expected, but %d parameters found",
				function->num_params, count);
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
 * @brief checks if two types are the same
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


