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
	if (list == NULL)
		return;
	indexList_t* lHead = NULL;
	GETLISTHEAD(list, lHead);

	irCode_t* irhead = NULL;
	irCode_t* ir = NULL;
	GETLISTHEAD(irListTail, irhead);
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
}

indexList_t* merge(indexList_t* l1, indexList_t* l2) {
	if (l1 == NULL)
		return l2;
	if (l2 == NULL)
		return l1;
	indexList_t* l1Tail = NULL;
	GETLISTTAIL(l1, l1Tail);
	indexList_t* l2Head = NULL;
	GETLISTHEAD(l2, l2Head);
	if (l1Tail != NULL) {
		l1Tail->next = l2Head;
	}
	if (l2Head != NULL) {
		l2Head->prev = l1Tail;
	}

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
	newList->next = NULL;
	newList->prev = NULL;
	return newList;
}

expr_t* newTmp(type_t type) {
	char* id = malloc(11 + 4);
	sprintf(id, "#V_%d", nextTmpVar);
	expr_t* newT = newExpr(id, type);
	var_t* var = createVar(id);
	var->type = type;
	if(type == T_INT) {
		var->width = 4;
	}
	insertVar(curSymbol, var);
	nextTmpVar++;
	return newT;
}

expr_t* newAnonymousExpr() {
	expr_t* newE = malloc(sizeof(expr_t));
	if (newE == NULL) {
		// TODO error
	}
	newE->value.id = "";
	newE->jump = 0;
	newE->type = T_UNKNOWN;
	newE->lvalue = 0; // FIXME Dirk do I have to do sth here??
	newE->valueKind = VAL_ID;
	newE->trueList = NULL;
	newE->falseList = NULL;
	newE->arrInd = NULL;
	newE->postEmit = PE_NONE;
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

stmt_t* newStmt() {
	stmt_t* stmt = malloc(sizeof(stmt_t));
	if (stmt == NULL) {
		// TODO error
	}
	stmt->nextList = NULL;
	return stmt;
}

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

	// TODO Nico res, arg0, arg1... expr -> arg

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
			// TODO Nico param list with findFunc()!!
			// R = CALL FUNC, (LISTE)
			fprintf(out, "<%.4d> %s = CALL %s, (%s)\n", nextIrCode->row, res,
					arg0, "");
			break;
		case OP_CALL_VOID:
			// TODO Nico param list
			// CALL FUNC, (LISTE)
			fprintf(out, "<%.4d> CALL %s, (%s)\n", nextIrCode->row, arg0, "");
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

		// free allocated mem for string rep. of numbers
//		if (res != NULL && nextIrCode->res != NULL && nextIrCode->res->valueKind == VAL_NUM) {
//			free(res);
//		}
//		if (arg0 != NULL && nextIrCode->arg0 != NULL && nextIrCode->arg0->valueKind == VAL_NUM) {
//			free(arg0);
//		}
//		if (arg1 != NULL && nextIrCode->arg1 != NULL && nextIrCode->arg1->valueKind == VAL_NUM) {
//			free(arg1);
//		}

		nextIrCode = nextIrCode->next;
	}
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
