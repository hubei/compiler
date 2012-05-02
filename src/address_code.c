#include "address_code.h"
#include <err.h>

/**
 * @brief Creates IRCode from parsed expressions
 * @param arg0
 * @param arg1
 * @param op
 */
expr_t* createIRCodeFromExpr(symbol_t* symTab, expr_t* arg0, operations_t op,
		expr_t* arg1) {
	irCode_t *newIRCode = (irCode_t*) malloc(sizeof(struct irCode_t));
	expr_t *resExpr = NULL;
	if (newIRCode == NULL) {
		err(1, "Could not allocate memory");
	}
	newIRCode->ops = op;
	if (arg0->valueKind == VAL_ID) {
		newIRCode->arg0.type = ARG_VAR;
		var_t* var = findVar(symTab, arg0->value.id);
		if (var != NULL) {
			newIRCode->arg0.arg._var = var;
		} else {
			printf("OH MANN, KEINE VARIABLE GEFUNDEN MIT DEM NAMEN %s!\n",
					arg0->value.id);
		}
	} else {
		newIRCode->arg0.type = ARG_CONST;
		newIRCode->arg0.arg._constant = arg0->value.num;
	}

	//Arg 1
	if (arg1->valueKind == VAL_ID) {
		newIRCode->arg1.type = ARG_VAR;
		if (findVar(symTab, arg1->value.id) != NULL) {
			//printf("VARIABLE(2) GEFUNDEN MIT DEM NAMEN %s!\n", arg0.value.id);
			newIRCode->arg1.arg._var = findVar(symTab, arg1->value.id);
		} else {
			printf("OH MANN, KEINE VARIABLE(2) GEFUNDEN MIT DEM NAMEN %s!\n",
					arg1->value.id);
		}
		//printf("%s = %i\n\n", arg0.value.id, arg1.value.id);
	} else {
		newIRCode->arg1.type = ARG_CONST;
		newIRCode->arg1.arg._constant = arg1->value.num;
	}
	//if irList was not assigned yet
	//--> irList = first node

	if (!irList) {
		irList = newIRCode;
		newIRCode->row = 0;
	} else {
		if (irList->next == NULL) {
			irList->next = newIRCode;
			newIRCode->prev = irList;
			newIRCode->row = 1;
			last = newIRCode;
		} else { //irList != NULL && irList->next != null
			newIRCode->prev = last;
			last->next = newIRCode;
			newIRCode->row = last->row + 1;
			last = newIRCode;
		}
	}

	//unless VAR = ....;
	//we'll create tmp-vars for each step
	//i.e. 	t0 = 1 + 1;
	//		a = t0;
	//
	// ---> res.arg._var = arg0.arg._const + arg1.arg._const;
	// ---> res.arg._var = this->prev->res.arg._var;
	if (op != OP_ASSIGN) {
		newIRCode->res.type = ARG_VAR;
		char tmp[0x10]; //16-stellen dürften mal dicke reichen! :D
		sprintf(tmp, "#V_%d", newIRCode->row);
		newIRCode->res.arg._var = createVar(tmp);
		newIRCode->res.arg._var->type = arg0->type;
		insertVar(symTab, newIRCode->res.arg._var);

		resExpr = malloc(sizeof(expr_t));

		resExpr->value.id = newIRCode->res.arg._var->id;
		resExpr->type = newIRCode->res.arg._var->type;
		resExpr->lvalue = 0; // FIXME Dirk do I have to do sth here??
		resExpr->valueKind = VAL_ID;
	} else {
		newIRCode->res = newIRCode->arg0;
		newIRCode->arg0 = newIRCode->arg1;
		resExpr = arg0;
	}

	return resExpr;
}

void printIRCode(FILE *out, irCode_t *irCode) {
	if (out == NULL)
		out = stdout;

	char* res;
	char* arg1;
	char* arg0;

	irCode_t *nextIrCode = irCode;
	while (nextIrCode != NULL) {
		res = getConstOrId(&nextIrCode->res);
		arg1 = getConstOrId(&nextIrCode->arg1);
		arg0 = getConstOrId(&nextIrCode->arg0);

		switch (nextIrCode->ops) {
		case OP_ASSIGN:
			fprintf(out, "<%.4d> %s = %s\n", nextIrCode->row, res, arg0);
			break;
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_MOD:
			fprintf(out, "<%.4d> %s = %s %s %s\n", nextIrCode->row, res, arg0,
					opToStr(nextIrCode->ops), arg1);
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
			fprintf(out, "<%.4d> IF %s %s %s GOTO %s\n", nextIrCode->row, arg0, opToStr(nextIrCode->ops), arg1, res);
			break;
		case OP_GOTO:
			fprintf(out, "<%.4d> GOTO %s\n", nextIrCode->row, res);
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
			fprintf(out, "<%.4d> %s = CALL %s, (%s)\n", nextIrCode->row, res, arg0, arg1);
			break;
		case OP_CALL_VOID:
			// TODO param list
			// CALL FUNC, (LISTE)
			fprintf(out, "<%.4d> CALL %s, (%s)\n", nextIrCode->row, arg0, arg1);
			break;
		case OP_ARRAY_LD:
			// R = ARR[IDX]
			fprintf(out, "<%.4d> %s = %s[%s]\n", nextIrCode->row, res, arg0, arg1);
			break;
		case OP_ARRAY_ST:
			// ARR[IDX] = S
			fprintf(out, "<%.4d> %s[%s] = %s\n", nextIrCode->row, res, arg0, arg1);
			break;
		default:
			// unkown ops, should not happen...
			break;
		}

		nextIrCode = nextIrCode->next;
	}
}

char* getConstOrId(irCode_arg_t* arg) {
	char* res = NULL;
	switch (arg->type) {
	case ARG_VAR:
		res = arg->arg._var->id;
		break;
	case ARG_FUNC:
		res = arg->arg._func->id;
		break;
	case ARG_CONST:
		res = malloc(11); // int has max 10 digits + end of string
		if (res == NULL)
			err(1, "Could not allocate");
		sprintf(res, "%d", arg->arg._constant);
		break;
	case ARG_UNKOWN:
	default:
		res = "";
		break;
	}

	assert(res!=NULL);
	return res;
}

char* opToStr(operations_t ops) {
	char* op = malloc(6);
	op = "";
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

irCode_t* getIRCode() {
	return irList;
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
