#include "address_code.h"

/**
 * @brief Creates IRCode from parsed expressions
 * @param arg0
 * @param arg1
 * @param op
 */
void createIRCodeFromExpr( expr arg0, int op, expr arg1 ) {
	irCode *x = (irCode*)malloc(sizeof(struct irCode));
	if(!x) {
		printf("ERROR IN ALLOCATING!");
		exit(-1);
	}
	x->ops = op;
	printf("Operation: %d\n", x->ops);
/*
	//Argument 0
	printf("ARGUMENTKIND: %d\ ", arg0.valueKind);
	if(arg0.valueKind==VAL_ID) {
		x->arg0.type = ARG_VAR;
		printf("TEST1");
		memcpy(x->arg0.arg._var,arg0.value.id,strlen(arg0.value.id));
		printf("TEST2");
	} else {
		x->arg0.type = ARG_CONST;
		printf("TEST1");
		memcpy(x->arg0.arg._constant,arg0.value.num,sizeof(int));
		printf("TEST2");
	}

	//Argument 1
	if(arg1.valueKind==VAL_ID) {
		x->arg1.type = ARG_VAR;
		printf("TEST3");
		memcpy(x->arg1.arg._var,arg1.value.id,strlen(arg1.value.id));
		printf("TEST4");
	} else {
		x->arg1.type = ARG_CONST;
		printf("TEST3");
		memcpy(x->arg1.arg._constant,arg1.value.num,sizeof(int));
		printf("TEST4");
		printf("CONST: %d\n", x->arg1.arg._constant);
	}
	printParam(x);*/
	free(x);
}

/**
 * @Brief Prints current argument (FOR TESTING PURPOSE ONLY)
 * @param line
 * @return
 */

void printParam( irCode *x ) {
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
	printf(const_str);
	//printf(const_str,x->arg0.type==ARG_CONST?x->arg0.arg._constant:(x->arg0.type==ARG_VAR?x->arg0.arg._var:x->arg0.arg._func));
}

/**
 * @brief Returns IRCode to String
 * @param line
 * @return
 */
string IRtoString( irCode *line ) {
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
char* OpToString( char *result, irCode *line, int op ) {
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

irCode* getIRCode() {
	irCode* result = NULL;
	return result;
}
