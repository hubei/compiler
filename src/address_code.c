#include "address_code.h"
#include "generalParserFunc.h"
#include "symboltable.h"
#include <errno.h>
#include "diag.h"
int instruction = 0;
int nextTmpVar = 0;
exprList_t* allExpr = NULL;

/**
 * @brief return the next instruction
 * The returned row number does *not* point to an existing irCode line
 * @return row number / instruction number
 */
int getNextInstr() {
	return instruction + 1;
}

/**
 * @brief set the index to all elements in list
 * @param list a list of indices
 * @param index index to set on the elements
 */
void backpatch(indexList_t* list, int index) {
	if (list == NULL)
		return;

	indexList_t* lHead = NULL;
	GETLISTHEAD(list, lHead);

	irCode_t* irhead = NULL;
	irCode_t* ir = NULL;

	GETLISTHEAD(curSymbol->ircode, irhead);
	// loop through all indices in given list
	while (lHead != NULL) {
		ir = irhead;
		// loop through all irCode until the row was found or the list ends
		while (ir != NULL) {
			if (ir->row == lHead->index) {
				// create new result expression to store the GOTO address
				ir->res = newAnonymousExpr();
				ir->res->jump = index;
				break;
			}
			ir = ir->next;
		}
		lHead = lHead->next;
	}
	clean_indexList(list);
}

/**
 * @brief Merge the two given lists into one
 * l1 and l2 can be NULL. So can the return value.
 * @param l1 list one
 * @param l2 list two
 * @return new merged list
 */
indexList_t* merge(indexList_t* l1, indexList_t* l2) {
	// if one of the list is NULL, just return the other
	if (l1 == NULL)
		return l2;
	if (l2 == NULL)
		return l1;

	// start with getting the correct start/end of the lists
	indexList_t* l1Tail = NULL;
	GETLISTTAIL(l1, l1Tail);
	indexList_t* l2Head = NULL;
	GETLISTHEAD(l2, l2Head);

	// concat the lists
	if (l1Tail != NULL) {
		l1Tail->next = l2Head;
	}
	if (l2Head != NULL) {
		l2Head->prev = l1Tail;
	}

	// get the list head, before returning it
	indexList_t* l1Head = NULL;
	GETLISTHEAD(l1, l1Head);
	return l1Head;
}

/**
 * @brief Create a new index list
 * @param i initial value to insert into list
 * @return new list
 */
indexList_t* newIndexList(int i) {
	indexList_t* newList = malloc(sizeof(indexList_t));
	if (newList == NULL) {
		// TODO error
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1, "new indexList could not be constructed");
	}
	newList->index = i;
	newList->next = NULL;
	newList->prev = NULL;
	return newList;
}

/**
 * @brief Create new tmp variable and insert into symTab
 * @param type data type of var
 * @return new tmp var
 */
expr_t* newTmp(type_t type) {
	// generate new id
	char* id = malloc(11 + 4);
	if (id == NULL) {
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1, "");
	}
	sprintf(id, "#V_%d", nextTmpVar);

	// new expr and var
	expr_t* newT = newExpr(id, type);
	var_t* var = createVar(id);
	var->type = type;
	if (type == T_INT) {
		var->width = 4;
	}
	// insert var into symTab
	insertVar(curSymbol, var);
	// increase number
	nextTmpVar++;
	// clean
	free(id);
	return newT;
}

/**
 * @brief Create a new expression (completely empty, without side-effects)
 * @return new expression
 */
expr_t* newAnonymousExpr() {
	expr_t* newE = malloc(sizeof(expr_t));
	if (newE == NULL) {
		// TODO error
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1, "new Expression could not be constructed");
	}

	// set default values
	newE->value.id = NULL;
	newE->jump = 0;
	newE->type = T_UNKNOWN;
	newE->lvalue = 0;
	newE->valueKind = VAL_UNKOWN;
	newE->trueList = NULL;
	newE->falseList = NULL;
	newE->arrInd = NULL;
	newE->postEmit = PE_NONE;
	newE->params = NULL;

	// add expr to allExpr list
	if (allExpr == NULL) {
		allExpr = newExprList(newE);
	} else {
		exprList_t* tmp = allExpr;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		tmp->next = newExprList(newE);
	}
	return newE;
}

/**
 * @brief Create a new expression list with the given single entry
 * @param expr first initial entry
 * @return list of expression with one entry
 */
exprList_t* newExprList(expr_t* expr) {
	exprList_t* newEL = malloc(sizeof(expr_t));

	if (newEL == NULL) {
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1,
				"adress_code.c", __LINE__, "new Expression could not be constructed");
	}

	newEL->expr = expr;
	newEL->next = NULL;
	newEL->prev = NULL;
	return newEL;
}

/**
 * @brief Create a new expression with given id and type
 * @param id name of variable/expression
 * @param type data type
 * @return new expression
 */
expr_t* newExpr(const char* id, type_t type) {
	expr_t* newE = newAnonymousExpr();
	char* newId = malloc(strlen(id) + 1);

	if (newId == NULL) {
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1,
				"adress_code.c", __LINE__, "new Expression could not be constructed");
	}

	strcpy(newId, id);
	newE->value.id = newId;
	newE->type = type;
	newE->valueKind = VAL_ID;
	return newE;
}

/**
 * @brief Create a new expression with given number and type
 * @param num value of expr/var
 * @param type data type
 * @return new expression
 */
expr_t* newExprNum(int num, type_t type) {
	expr_t* newE = newAnonymousExpr();
	newE->value.num = num;
	newE->type = type;
	newE->valueKind = VAL_NUM;
	return newE;
}

/**
 * @brief Create a new empty statement
 * currently only stores a nextList
 * @return new Stmt
 */
stmt_t* newStmt() {
	stmt_t* stmt = malloc(sizeof(stmt_t));
	if (stmt == NULL) {
		// TODO error
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1,
				"adress_code.c", __LINE__, "new Statement could not be constructed");

	}
	stmt->nextList = NULL;
	stmt->returnType = T_UNKNOWN;
	return stmt;
}

/**
 * @brief Delete last instruction line and decrease counter
 */
void delLastInstr() {
	if (curSymbol->ircode != NULL) {
		irCode_t* lastIR = curSymbol->ircode;
		curSymbol->ircode = curSymbol->ircode->prev;
		destroyVar(curSymbol, lastIR->res->value.id);
		free(lastIR);
		instruction--;
	}
}

/**
 * @brief Creates a new IRCode line with given arguments and the operator
 * @param res
 * @param arg0
 * @param op
 * @param arg1
 */
void emit(expr_t* res, expr_t* arg0, operation_t op, expr_t* arg1) {

	// create a new ircode line
	irCode_t *newIRCode = (irCode_t*) malloc(sizeof(struct irCode_t));
	if (newIRCode == NULL) {
		// TODO error
		FATAL_OS_ERROR(OUT_OF_MEMORY, 1,
				"adress_code.c", __LINE__, "new IR code Line could not be constructed");
	}

	// initialize
	newIRCode->ops = op;
	newIRCode->next = NULL;
	newIRCode->prev = NULL;
	newIRCode->arg0 = arg0;
	newIRCode->arg1 = arg1;
	newIRCode->res = res;

	// insert into list
	if (curSymbol->ircode != NULL) {
		curSymbol->ircode->next = newIRCode;
	}
	newIRCode->prev = curSymbol->ircode;
	newIRCode->row = getNextInstr();
	curSymbol->ircode = newIRCode; // new tail
	instruction++; // one more instruction
}

/**
 * @brief print the given irCode list into given file or to stdout, if file is NULL
 * @param out an opened file or NULL for stdout
 * @param irCode an irCode list HEAD
 */
void printIRCode(FILE *out, irCode_t *irCode) {
	if (out == NULL)
		out = stdout;
	if (irCode == NULL)
		return;

	// store string representations of the args
	char* res = NULL;
	char* arg1 = NULL;
	char* arg0 = NULL;
	char* exprL = NULL;

	irCode_t *nextIrCode = NULL;
	GETLISTHEAD(irCode, nextIrCode);
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
		case OP_MOD:
		case OP_SHL:
		case OP_SHR: {
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
			fprintf(out, "<%.4d> GOTO %d\n", nextIrCode->row,
					nextIrCode->res->jump);
			break;
		case OP_RETURN_VAL:
			fprintf(out, "<%.4d> RETURN %s\n", nextIrCode->row, res);
			break;
		case OP_RETURN_VOID:
			fprintf(out, "<%.4d> RETURN\n", nextIrCode->row);
			break;
		case OP_CALL_RES:
			// R = CALL FUNC, (LISTE)
			exprL = exprListToStr(nextIrCode->res == NULL ? NULL : nextIrCode->res->params);
			fprintf(out, "<%.4d> %s = CALL %s, (%s)\n", nextIrCode->row, res,
					arg0, exprL);
			free(exprL);
			break;
		case OP_CALL_VOID:
			// CALL FUNC, (LISTE)
			exprL = exprListToStr(nextIrCode->arg0 == NULL ? NULL : nextIrCode->arg0->params);
			fprintf(out, "<%.4d> CALL %s, (%s)\n", nextIrCode->row, arg0,
					exprL);
			free(exprL);
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
			// unkown ops, should not happen... :)
			break;
		}

		// we do not need the strings anymore
		free(res);
		free(arg0);
		free(arg1);

		nextIrCode = nextIrCode->next;
	}
}

/**
 * @brief Convert a expression list to string (comma separated)
 * @param el exprList
 * @return String
 */
char* exprListToStr(exprList_t* el) {
	char* result = NULL;
	if (el == NULL) {
		// we return an empty string, but we'll allocate memory
		// so that it can be free'd without a special case
		result = malloc(1);

		if (result == NULL) {
			FATAL_OS_ERROR(OUT_OF_MEMORY, 1, "adress_code.c", __LINE__, "");
		}

		strcpy(result, "");
		return result;
	}
	exprList_t* tmpEl = NULL;
	GETLISTHEAD(el, tmpEl);
	while (tmpEl != NULL) {
		if (result == NULL) {
			result = valueAsString(tmpEl->expr);
		} else {
			char* new = valueAsString(tmpEl->expr);
			char* old = result;
			result = malloc(strlen(new) + strlen(old) + 2);
			if (result == NULL) {
				FATAL_OS_ERROR(OUT_OF_MEMORY, 1,
						"adress_code.c", __LINE__, "new Expression could not be constructed");
			}
			sprintf(result, "%s,%s", old, new);
		}
		tmpEl = tmpEl->next;
	}
	return result;
}

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
	case OP_SHL:
		op = "<<";
		break;
	case OP_SHR:
		op = ">>";
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
 * @brief return list of all irCodes
 * TODO Nicolai remove
 * @return irCode HEAD
 */
irCode_t* getIRCode() {
	irCode_t* head = NULL;
	if (curSymbol->ircode != NULL) {
		GETLISTHEAD(curSymbol->ircode, head);
	}
	return head;
}

/**
 * @brief loop through the list of all expression and clean each one
 */
void clean_all_expr() {
	while (allExpr != NULL) {
		exprList_t* tmp = allExpr;
		allExpr = allExpr->next;
		clean_expr(tmp->expr);
		free(tmp);
	}
}

/**
 * @brief clean a complete expression list
 * @param exprList
 */
void clean_exprList(exprList_t* exprList) {
	while (exprList != NULL) {
		exprList_t* tmp = exprList;
		exprList = exprList->next;
		free(tmp);
	}
}

/**
 * @brief free memory of a expression
 * Hint: Most containing data is free'd elsewhere<br>
 * e.g. parentId is a reference to id of parent -> do not free
 * @param expr
 */
void clean_expr(expr_t* expr) {
	if (expr == NULL)
		return;
	if (expr->valueKind == VAL_ID) {
		if (expr->value.id != NULL) {
			free(expr->value.id);
		}
	}
	clean_exprList(expr->params);
	free(expr);
}
