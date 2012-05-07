#include "address_code.h"
#include "generalParserFunc.h"
#include <err.h>

// always point to the next instruction
int instruction = 0;
int nextTmpVar = 0;

int getNextInstr() {
	return instruction + 1;
}

/**
 * @brief set the index to all elements in list
 * @param list a list of indices
 * @param index index to set on the elements
 */
void backpatch(indexList_t* list, int index) {
	indexList_t* lHead = NULL;
	GETLISTHEAD(list, lHead);

	irCode_t* irhead = NULL;
	irCode_t* ir = NULL;
	GETLISTHEAD(irListTail, irhead);
	// loop through all indices in given list
	while (lHead != NULL) {
		ir = irhead;
		// loop through all irCode until the row was found or the list ends
		while(ir != NULL) {
			if(ir->row == lHead->index) {
				// create new result expression to store the GOTO address
				ir->res = malloc(sizeof(expr_t));
				if(ir->res == NULL) {
					// TODO error
				} else {
					ir->res->jump = index;
				}
				break;
			}
			ir = ir->next;
		}
		lHead = lHead->next;
	}
}

indexList_t* merge(indexList_t* l1, indexList_t* l2) {
	indexList_t* l1Tail = NULL;
	GETLISTTAIL(l1, l1Tail);
	indexList_t* l2Head = NULL;
	GETLISTHEAD(l2, l2Head);
	l1Tail->next = l2Head;
	l2Head->prev = l1Tail;

	indexList_t* l1Head = NULL;
	GETLISTHEAD(l1, l1Head);
	return l1Head;
}

indexList_t* createList(int i) {
	indexList_t* newList = malloc(sizeof(indexList_t));
	if (newList == NULL) {
		// TODO error
	}
	newList->index = i;
	return newList;
}

expr_t* newTmp() {
	char* id = malloc(11 + 4);
	sprintf(id, "#V_%d", nextTmpVar);
	expr_t* newT = newExpr(id, T_INT);
	insertVar(curSymbol, createVar(id));
	nextTmpVar++;
	return newT;
}

expr_t* newAnonymousExpr() {
	expr_t* newE = malloc(sizeof(expr_t));
	if(newE == NULL) {
		// TODO error
	}
	newE->value.id = "";
	newE->jump = 0;
	newE->type = T_UNKNOWN;
	newE->lvalue = 0; // FIXME Dirk do I have to do sth here??
	newE->valueKind = VAL_ID;
	newE->trueList = NULL;
	newE->falseList = NULL;
	return newE;
}

expr_t* newExpr(char* id, type_t type) {
	expr_t* newE = newAnonymousExpr();
	newE->value.id = id;
	newE->type = type;
	newE->valueKind = VAL_ID;
	return newE;
}

expr_t* newExprNum(int num, type_t type) {
	expr_t* newE = newAnonymousExpr();
	newE->value.num = num;
	newE->type = type;
	newE->valueKind = VAL_NUM;
	return newE;
}

//irCode_arg_t* argFromExpr(expr_t* expr) {
//	irCode_arg_t* arg = malloc(sizeof(irCode_arg_t));
//	if(arg == NULL) {
//		// TODO error
//	}
//	if (expr->valueKind == VAL_ID) {
//		arg->type = ARG_VAR;
//		var_t* var = findVar(curSymbol, expr->value.id);
//		if (var != NULL) {
//			arg->arg._var = var;
//		} else {
//			// TODO throw error
//			printf("OH MANN, KEINE VARIABLE GEFUNDEN MIT DEM NAMEN %s!\n",
//					expr->value.id);
//		}
//	} else {
//		arg->type = ARG_CONST;
//		arg->arg._constant = expr->value.num;
//	}
//	return arg;
//}

/**
 * @brief Creates IRCode from parsed expressions
 * @param arg0
 * @param arg1
 * @param op
 */
void emit(expr_t* res, expr_t* arg0, operation_t op, expr_t* arg1) {

	// create a new ircode line
	irCode_t *newIRCode = (irCode_t*) malloc(sizeof(struct irCode_t));
	if (newIRCode == NULL) {
		err(1, "Could not allocate memory");
	}

	// initialize
	newIRCode->ops = op;
	newIRCode->next = NULL;
	newIRCode->prev = NULL;
	newIRCode->label = NULL;
	newIRCode->arg0 = arg0;
	newIRCode->arg1 = arg1;
	newIRCode->res = res;

	// TODO res, arg0, arg1... expr -> arg

//	// Arg 0
//	if (arg0Expr != NULL) {
//		newIRCode->arg0 = argFromExpr(arg0Expr);
//	}
//	// Arg 1
//	if (arg1Expr != NULL) {
//		newIRCode->arg1 = argFromExpr(arg1Expr);
//	}
//	// res
//	if (resExpr != NULL) {
//		newIRCode->res = argFromExpr(resExpr);
//	}

	// insert into list
	if (irListTail != NULL) {
		irListTail->next = newIRCode;
	}
	newIRCode->prev = irListTail;
	newIRCode->row = getNextInstr();
	irListTail = newIRCode; // new tail
	instruction++;
}

/**
 * @brief print the given irCode list into given file or to stdout, if file is NULL
 * @param out an opened file or NULL for stdout
 * @param irCode an irCode list HEAD
 */
void printIRCode(FILE *out, irCode_t *irCode) {
	if (out == NULL)
		out = stdout;

	char* res = NULL;
	char* arg1 = NULL;
	char* arg0 = NULL;

	irCode_t *nextIrCode = irCode;
	// for each irCode line
	while (nextIrCode != NULL) {
		res = valueAsString(nextIrCode->res);
		arg1 = valueAsString(nextIrCode->arg1);
		arg0 = valueAsString(nextIrCode->arg0);

		switch (nextIrCode->ops) {
		case OP_ASSIGN:
			fprintf(out, "<%.4d> %s = %s\n", nextIrCode->row, res, arg0);
			break;
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_MOD: {
			fprintf(out, "<%.4d> %s = %s %s %s\n", nextIrCode->row, res, arg0,
					opToStr(nextIrCode->ops), arg1);
		}
			break;
		case OP_MINUS:
			fprintf(out, "<%.4d> %s = - %s\n", nextIrCode->row, res, arg0);
			break;
		case OP_IFEQ:
		case OP_IFNE:
		case OP_IFGT:
		case OP_IFGE:
		case OP_IFLT:
		case OP_IFLE:
			fprintf(out, "<%.4d> IF %s %s %s GOTO %d\n", nextIrCode->row, arg0,
					opToStr(nextIrCode->ops), arg1, nextIrCode->res->jump);
			break;
		case OP_GOTO:
			fprintf(out, "<%.4d> GOTO %d\n", nextIrCode->row, nextIrCode->res->jump);
			break;
		case OP_RETURN_VAL:
			fprintf(out, "<%.4d> RETURN %s\n", nextIrCode->row, res);
			break;
		case OP_RETURN_VOID:
			fprintf(out, "<%.4d> RETURN\n", nextIrCode->row);
			break;
		case OP_CALL_RES:
			// TODO param list
			// R = CALL FUNC, (LISTE)
			fprintf(out, "<%.4d> %s = CALL %s, (%s)\n", nextIrCode->row, res,
					arg0, arg1);
			break;
		case OP_CALL_VOID:
			// TODO param list
			// CALL FUNC, (LISTE)
			fprintf(out, "<%.4d> CALL %s, (%s)\n", nextIrCode->row, arg0, arg1);
			break;
		case OP_ARRAY_LD:
			// R = ARR[IDX]
			fprintf(out, "<%.4d> %s = %s[%s]\n", nextIrCode->row, res, arg0,
					arg1);
			break;
		case OP_ARRAY_ST:
			// ARR[IDX] = S
			fprintf(out, "<%.4d> %s[%s] = %s\n", nextIrCode->row, res, arg0,
					arg1);
			break;
		default:
			// unkown ops, should not happen...
			break;
		}

		// free allocated mem for string rep. of numbers
//		if (nextIrCode->res->type == ARG_CONST) {
//			free(res);
//		}
//		if (nextIrCode->arg0->type == ARG_CONST) {
//			free(arg0);
//		}
//		if (nextIrCode->arg1->type == ARG_CONST) {
//			free(arg1);
//		}

		nextIrCode = nextIrCode->next;
	}
}

///**
// * @brief Get a string representation of the given argument
// * argument can be a function, variable or constant (number)
// * @param arg irCode_arg_t
// * @return function/variable name or constant number as string
// */
//char* getConstOrId(expr_t* expr) {
//	char* res = NULL;
//	// FIXME get code from Dirk
////	switch (expr->valueKind) {
////	case ARG_FUNC:
////		res = arg->arg._func->id;
////		break;
////	case ARG_CONST:
////		res = malloc(11); // int has max 10 digits + end of string
////		if (res == NULL)
////			err(1, "Could not allocate");
////		sprintf(res, "%d", arg->arg._constant);
////		break;
////	case ARG_UNKOWN:
////	default:
////		res = "";
////		break;
////	}
////
////	assert(res!=NULL);
//	return res;
//}

/**
 * @brief get a string representation of the given operand
 * @param ops operation
 * @return string
 */
char* opToStr(operation_t ops) {
	char* op = "";
	switch (ops) {
	case OP_ASSIGN:
		break;
	case OP_ADD:
		op = "+";
		break;
	case OP_SUB:
		op = "-";
		break;
	case OP_MUL:
		op = "*";
		break;
	case OP_DIV:
		op = "/";
		break;
	case OP_MOD:
		op = "%";
		break;
	case OP_MINUS:
		op = "-";
		break;
	case OP_IFEQ:
		op = "==";
		break;
	case OP_IFNE:
		op = "!=";
		break;
	case OP_IFGT:
		op = ">";
		break;
	case OP_IFGE:
		op = ">=";
		break;
	case OP_IFLT:
		op = "<";
		break;
	case OP_IFLE:
		op = "<=";
		break;
	case OP_GOTO:
		op = "goto";
		break;
	case OP_RETURN_VAL:
	case OP_RETURN_VOID:
	case OP_CALL_RES:
	case OP_CALL_VOID:
	case OP_ARRAY_LD:
	case OP_ARRAY_ST:
		break;
	}
	return op;
}

/**
 * return list of all irCodes
 * @return
 */
irCode_t* getIRCode() {
	irCode_t* head = NULL;
	GETLISTHEAD(irListTail, head);
	return head;
}

///**
// * @Brief Prints current argument (FOR TESTING PURPOSE ONLY)
// * @param line
// * @return
// */
//void printParam(irCode_t *x) {
//	if (!x)
//		return;
//	char const_str[100] = "Values: ";
//	if (x->arg0.type == ARG_CONST) {
//		strcat(const_str, "%d, ");
//	} else {
//		strcat(const_str, "%s, ");
//	}
//
//	if (x->arg1.type == ARG_CONST) {
//		strcat(const_str, "%d ");
//	} else {
//		strcat(const_str, "%s ");
//	}
////	printf(const_str,x->arg0.type==ARG_CONST?x->arg0.arg._constant:(x->arg0.type==ARG_VAR?x->arg0.arg._var:x->arg0.arg._func));
//}

///**
// * @brief Returns IRCode to String
// * @param line
// * @return
// */
//string IRtoString(irCode_t *line) {
//	char* result = (char*) malloc(1);
//	if (!line)
//		return "";
//	result = lineToString(result, line);
//	return (string) result;
//}

///**
// * @brief automatically extends dest if size is too small and appends chars.
// * @param dest
// * @param source
// * @return
// */
//char* concat(char* dest, char *source) {
//	assert(dest!=NULL);
//	assert(source!=NULL);
//	int newlen = strlen(dest) + strlen(source);
//	int maxsize = sizeof(dest);
//	if (newlen >= maxsize) {
//		char *tmp = NULL;
//		if (strlen(dest) > 0) {
//			char *tmp = (char*) malloc(maxsize); //temp-buffer for *dest*
//			strcpy(tmp, dest); //Copy content from dest to *tmp*
//		}
//		dest = (char*) realloc(dest, newlen + 1); //Reallocate dest
//		if (!dest) {
//			err(1, "Could not allocate memory");
//		}
//		if (strlen(dest) > 0 && tmp != NULL) {
//			strcpy(dest, tmp); //re-move from temp-buffer to *dest*
//			free(tmp); //free tmp and NULL it
//			tmp = NULL;
//		}
//	}
//	strcat(dest, source); //Append new source
//	return dest;
//}
//
///**
// * @brief Chooses Operation and converts it into a string
// * @param result
// * @param line
// * @param op
// * @return
// */
//char* lineToString(char *result, irCode_t *line) {
//	// alternative:
////	result = malloc(100);
////	sprintf(result, "<%.4d> %s = %s\n", line->row, line->res.arg._var->id,
////			line->arg0.arg._var->id);
//	//Result
//	//X = a operator b
//	char cur_line[12] = "";
//	//sprintf(cur_line,"%s",line->row);
//
//	switch (line->ops) {
//	case OP_ASSIGN: {
//		//Format: X = ....
//		if (line->res.arg._var != NULL || line->arg0.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, ";");
//		}
//	}
//		break;
//	case OP_ADD: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			//<LINE>: a = 1	+ 2;
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " + ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, ";");
//		}
//	}
//		break;
//	case OP_SUB: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " - ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, ";");
//		}
//	}
//		break;
//	case OP_MUL: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " * ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, ";");
//		}
//	}
//		break;
//	case OP_DIV: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " * ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, ";");
//		}
//	}
//		break;
//	case OP_MINUS: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, line->res.arg._var->id);
//			concat(result, " = -");
//			concat(result, line->arg0.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFEQ: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " == ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFNE: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " != ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFGT: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " > ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFGE: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " >= ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFLT: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " < ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_IFLE: {
//		if (line->res.arg._var == NULL || line->arg0.arg._var == NULL
//				|| line->arg1.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "IF ");
//			concat(result, line->arg0.arg._var->id);
//			concat(result, " <= ");
//			concat(result, line->arg1.arg._var->id);
//			concat(result, " GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	case OP_GOTO: {
//		if (line->res.arg._var == NULL)
//			return "";
//		if (line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL
//				&& line->arg1.arg._var->id != NULL) {
//			concat(result, "<");
//			concat(result, cur_line);
//			concat(result, ">: ");
//			concat(result, "GOTO ");
//			concat(result, line->res.arg._var->id);
//		}
//	}
//		break;
//	}
//	return result;
//}
