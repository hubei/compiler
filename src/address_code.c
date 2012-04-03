#include "address_code.h"
#include "types.h"

/**
 * TODO: Convert an entire IRCode-Line to string
 * @return
 */

string IRtoString( irCode *line ) {

	/*
	int len = strlen(line->res.arg._var->id);
			  strlen((line->arg0.type==ARG_VAR?line->arg0.arg->_var->id:
			  	  	  line->arg0.type==ARG_FUNC?line->arg0.arg->_func->id:"")) +

			  strlen((line->arg1.type==ARG_VAR?line->arg1.arg->_var->id:
			   	  	  line->arg1.type==ARG_FUNC?line->arg1.arg->_func->id:""));
	*/
	char* result = malloc(sizeof("TEST"));
	return result;
}

string OpToString( int op ) {
	switch(op) {
		case OP_ASSIGN:
			return "=";
		case OP_ADD:
			return "+";
		case OP_MINUS:
		case OP_SUB:
			return "-";
		case OP_MUL:
			return "*";
		case OP_DIV:
			return "/";
		case OP_IFEQ:
			return "if =";
		case OP_IFNE:
			return "if !=";
		case OP_IFGT:
			return "if >";
		case OP_IFGE:
			return "if >=";
		case OP_IFLT:
			return "if <";
		case OP_IFLE:
			return "if <=";
		case OP_GOTO:
			return "GOTO";
		case OP_RETURN_VAL:
			return "return ";
		case OP_RETURN_VOID:
			return "return";
		case OP_CALL_RES:
			return "... = CALL FUNC ";
		case OP_CALL_VOID:
			return "CALL FUNC";
		case OP_ARRAY_LD:
			return "... = ARRAY[X]";
		case OP_ARRAY_ST:
			return "ARRAY[X] = ...";
	}
	return "";
}

irCode* getIRCode() {
	irCode* result = NULL;
	return result;
}
