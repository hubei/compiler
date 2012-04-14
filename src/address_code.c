#include "address_code.h"

/**
 * @brief Creates IRCode from parsed expressions
 * @param arg0
 * @param arg1
 * @param op
 */
void createIRCodeFromExpr( symbol_t* symTab, expr_t arg0, int op, expr_t arg1 ) {
	irCode_t *x = (irCode_t*)malloc(sizeof(struct irCode_t));
	assert(x!=NULL);
	x->ops = op;
	//Argument 0
	if(arg0.valueKind==VAL_ID) {
		x->arg0.type = ARG_VAR;
		if(findVar(symTab,arg0.value.id)!=NULL) {
		} else {
			printf("OH MANN, KEINE VARIABLE GEFUNDEN MIT DEM NAMEN %s!\n", arg0.value.id);
		}
		x->arg0.arg._var = findVar(symTab,arg0.value.id);
	} else {
		//printf("..........\n");
	}

	if(arg1.valueKind==VAL_ID) {
		x->arg1.type = ARG_VAR;
		if(findVar(symTab,arg1.value.id)!=NULL) {
		} else {
			printf("OH MANN, KEINE VARIABLE GEFUNDEN MIT DEM NAMEN %s!\n", arg1.value.id);
		}
		x->arg1.arg._var = findVar(symTab,arg1.value.id);
		//printf("%s = %i\n\n", arg0.value.id, arg1.value.id);
	} else {
		x->arg1.type = ARG_CONST;
		x->arg1.arg._constant = arg1.value.num;
		//printf("%s = %i\n\n", arg0.value.id, x->arg1.arg._constant);
	}
	free(x);
	x=NULL;
}

/**
 * @Brief Prints current argument (FOR TESTING PURPOSE ONLY)
 * @param line
 * @return
 */

void printParam( irCode_t *x ) {
	if(!x)
		return;
	char const_str[100] = "Values: ";
	if(x->arg0.type == ARG_CONST) {
		strcat(const_str,"%d, ");
	} else {
		strcat(const_str,"%s, ");
	}

	if(x->arg1.type == ARG_CONST) {
		strcat(const_str,"%d ");
	} else {
		strcat(const_str,"%s ");
	}
//	printf(const_str,x->arg0.type==ARG_CONST?x->arg0.arg._constant:(x->arg0.type==ARG_VAR?x->arg0.arg._var:x->arg0.arg._func));
}

/**
 * @brief Returns IRCode to String
 * @param line
 * @return
 */
string IRtoString( irCode_t *line ) {
	char* result = (char*)malloc(1);
	if(!line)
		return "";
	result = OpToString(result,line,line->ops);
	return (string)result;
}

/**
 * @brief automatically extends dest if size is too small and appends chars.
 * @param dest
 * @param source
 * @return
 */
char* concat( char* dest, char *source ) {
	int newlen = strlen(dest)+strlen(source);
	int maxsize = sizeof(dest)/sizeof(dest[0]);
	if(newlen>=maxsize) {
		char *tmp = NULL;
		if(strlen(dest)>0) {
			char *tmp = (char*)malloc(maxsize); //temp-buffer for *dest*
			strcpy(tmp,dest);					//Copy content from dest to *tmp*
		}
		dest = (char*)realloc(dest,newlen+1);	//Reallocate dest
		if(!dest) {
			return NULL;
		}
		if(strlen(dest)>0 && tmp!=NULL) {
			strcpy(dest,tmp);					//re-move from temp-buffer to *dest*
			free(tmp);							//free tmp and NULL it
			tmp=NULL;
		}
	}
	strcat(dest,source);					//Append new source
	return dest;
}

/**
 * @brief Chooses Operation and converts it into a string
 * @param result
 * @param line
 * @param op
 * @return
 */
char* OpToString( char *result, irCode_t *line, int op ) {
	//Result
	//X = a operator b
	char cur_line[12] = "";
	//sprintf(cur_line,"%s",line->row);

	switch(line->ops) {
		case OP_ASSIGN: {
				//Format: X = ....
			if(line->res.arg._var!=NULL || line->arg0.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id != NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = ");
				concat(result,line->arg0.arg._var->id);
				concat(result,";");
			}
		}
		break;
		case OP_ADD: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = ");
				concat(result,line->arg0.arg._var->id);
				concat(result," + ");
				concat(result,line->arg1.arg._var->id);
				concat(result,";");
			}
		}
		break;
		case OP_SUB: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = ");
				concat(result,line->arg0.arg._var->id);
				concat(result," - ");
				concat(result,line->arg1.arg._var->id);
				concat(result,";");
			}
		}
		break;
		case OP_MUL: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = ");
				concat(result,line->arg0.arg._var->id);
				concat(result," * ");
				concat(result,line->arg1.arg._var->id);
				concat(result,";");
			}
		}
		break;
		case OP_DIV: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = ");
				concat(result,line->arg0.arg._var->id);
				concat(result," * ");
				concat(result,line->arg1.arg._var->id);
				concat(result,";");
			}
		}
		break;
		case OP_MINUS: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,line->res.arg._var->id);
				concat(result," = -");
				concat(result,line->arg0.arg._var->id);
			}
		}
		break;
		case OP_IFEQ: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," == ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_IFNE: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," != ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_IFGT: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," > ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_IFGE: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," >= ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_IFLT: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," < ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_IFLE: {
			if(line->res.arg._var==NULL || line->arg0.arg._var==NULL || line->arg1.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"IF ");
				concat(result,line->arg0.arg._var->id);
				concat(result," <= ");
				concat(result,line->arg1.arg._var->id);
				concat(result," GOTO " );
				concat(result,line->res.arg._var->id);
			}
		}
		break;
		case OP_GOTO: {
			if(line->res.arg._var==NULL)
				return "";
			if(line->res.arg._var->id != NULL && line->arg0.arg._var->id!=NULL && line->arg1.arg._var->id!=NULL) {
				concat(result,"<");
				concat(result,cur_line);
				concat(result,">: ");
				concat(result,"GOTO ");
				concat(result,line->res.arg._var->id);
			}
		}
		break;
	}
	return result;
}

irCode_t* getIRCode() {
	irCode_t* result = NULL;
	return result;
}
