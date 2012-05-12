/** 
 * @file parser.y 
 * @brief Parser utility for the DHBW compiler!
 */
 
%{
#include <stdio.h>
#include <stdlib.h>
#include "symboltable.h"
#include "typechecking.h"
#include "address_code.h" 
#include "generalParserFunc.h"

#define YYERROR_VERBOSE
	
	
%}

/**
 * TODO
 */
%debug
/**
 * TODO
 */
%locations
/**
 * TODO
 */
%error-verbose
/**
 * Set start point
 */
%start program

%code requires {
	#include "types.h"
}

/**
 * Define types for numericals and strings
 */
%union {
	char* str;
	int num;
	var_t* var;
	func_t* func;
	param_t* param;
	type_t type;
	struct {
		type_t type;
		int width;
	} typeExt;
	exprList_t *exprList;
	expr_t *expr;
	stmt_t *statmt;
	struct {
		int instr;
	} instr;
}

/*
 * One shift/reduce conflict is expected for the "dangling-else" problem. This
 * conflict however is solved by the default behavior of bison for shift/reduce 
 * conflicts (shift action). The default behavior of bison corresponds to what
 * we want bison to do: SHIFT if the lookahead is 'ELSE' in order to bind the 'ELSE' to
 * the last open if-clause. 
 */
%expect 1

%token DO WHILE
%token IF ELSE
%token INT VOID
%token RETURN
%token COLON COMMA SEMICOLON
%token BRACE_OPEN BRACE_CLOSE
%token END_OF_FILE

%token <str>ID
%token <num>NUM

%token ASSIGN
%token LOGICAL_OR LOGICAL_NOT LOGICAL_AND 
%token EQ NE LS LSEQ GTEQ GT
%token PLUS MINUS MUL DIV MOD
%token NOT UNARY_MINUS UNARY_PLUS
%token BRACKET_OPEN BRACKET_CLOSE PARA_OPEN PARA_CLOSE

// source: http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
/*%left COMMA not needed */
%right ASSIGN
%left LOGICAL_OR
%left LOGICAL_AND
%left EQ NE 
%left LS LSEQ GTEQ GT
%left SHIFT_LEFT SHIFT_RIGHT
%left PLUS MINUS
%left MUL DIV MOD 
%right LOGICAL_NOT NOT UNARY_MINUS UNARY_PLUS
%left BRACKET_OPEN BRACKET_CLOSE PARA_OPEN PARA_CLOSE

%type <var>identifier_declaration function_parameter
%type <param>function_parameter_list
%type <expr>expression function_call primary
%type <typeExt>type variable_declaration
%type <exprList>function_call_parameters
%type <statmt>stmt stmt_conditional stmt_loop stmt_block
%type <instr>M

%%

/* 
 * The start symbol of the programming language. According to the
 * original GNU C compiler (GCC), there has to be at least one
 * program_element. The End-of-File token is necessary for completing
 * the parsing process successfully by reducing to the start symbol
 * of grammar 'program'. 
 */
program
     : {curSymbol = getSymbolTable();} program_element_list {  }
     ;

/*
 * The list of program elements which are considered as high-level elements like function
 * definitions, global (constant) variables. The non-terminal 'program_element_list' consists at 
 * least of one program element. Though, empty source files will not succeed.
 */									
program_element_list
     : program_element_list program_element  { }
     | program_element { }
     ;

/*
 * The different program elements which are variable declarations, function definitions and
 * function prototypes and type definitions for the basic version of the compiler. 
 */									
program_element
     : variable_declaration SEMICOLON { }
     | function_declaration SEMICOLON { }
     | function_definition { }
     | SEMICOLON { }
     ;
									
/* 
 * The different types of the subset including self-defined data types by using the typedef
 * instruction.
*/
type
     : INT {  $$.type = T_INT; $$.width = 4; }
     | VOID {  $$.type = T_VOID; $$.width = 0; }
     ;

/*
 * 
 */
variable_declaration
	: variable_declaration COMMA identifier_declaration	 {
		
		$3->type = $1.type;
		$3->width = $1.width;
		if($3->size != 0) {
			$3->type = T_INT_A;
			$3->width = $3->size * $1.width;
		}
		insertVar(curSymbol, $3);
	}
	| type identifier_declaration {
		
		if($2 == NULL)
			error("variable_declaration: $2 is NULL");
		$2->type = $1.type;
		$2->width = $1.width;
		if($2->size != 0) {
			$2->type = T_INT_A;
			$2->width = $2->size * $1.width;
		}
		insertVar(curSymbol, $2);
		$$ = $1;
	}
	;

/*
 * The non-terminal 'identifier_declaration' contains the specifics of the variable beside
 * the type definition like arrays, pointers and initial (default) values.
 */									
identifier_declaration
     : ID BRACKET_OPEN NUM BRACKET_CLOSE { /* BRACKET = [] !!! */
    	  
    	 var_t* newVar = createVar($1);
    	 if(newVar == NULL) {
    		 error("identifier_declaration: newVar is NULL");
    	 }
    	 $$ = newVar;
		 $$->size = $3;
     } 
     | ID {	
    	 
    	 var_t* newVar = createVar($1);
    	 if(newVar == NULL) {
    		 error("identifier_declaration: newVar is NULL");
    	 }
    	 $$ = newVar;
    	 $$->size=0; 
     }
     ;
 	
 /*
 * 
 */
function_declaration
	: type ID PARA_OPEN PARA_CLOSE {
		 
		func_t* newFunc = createFunc($2);
		if(newFunc==NULL) {
			error("function_declaration: newFunc is NULL");
		}
		newFunc->returnType = $1.type;
		newFunc->param = NULL;
		insertFunc(curSymbol, newFunc);
	}
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		
		func_t* newFunc = createFunc($2);
		if(newFunc==NULL) {
			error("function_declaration: newFunc is NULL");
		}
		newFunc->returnType = $1.type;
		GETLISTHEAD($4, newFunc->param);
		newFunc->param = newFunc->param;
		newFunc->num_params = getParamCount(newFunc->param);
		insertFunc(curSymbol, newFunc);
	}
	;

/*
 * 
 */
function_parameter_list
	: function_parameter {
		
		$$ = addParam(NULL, $1);
	}
	| function_parameter_list COMMA function_parameter {
		
		$$ = addParam($1, $3);
	}
	;

/*
 * 
 */
function_parameter
	: type identifier_declaration {
		
		if($2==NULL) {
			error("function_parameter: $2 is NULL");
		}
		$2->type = $1.type;
		$2->width = $1.width;
		if($2->size != 0) {
			$2->type = T_INT_A;
			$2->width = $2->size * $1.width;
		}
		$$ = $2;
	}
	;
	
/*
 * The non-terminal 'function_definition' is the beginning of the function definition.
 */
function_definition
	: type ID PARA_OPEN PARA_CLOSE {
		func_t* newFunc = createFunc($2);
		if(newFunc==NULL) {
			error("function_definition: newFunc is NULL");
		}
		newFunc->returnType = $1.type;
		newFunc->param = NULL;
		if(exists(curSymbol,$2)) {
			func_t* existingFunc = findFunc(curSymbol, $2);
			if(existingFunc == NULL) {
				error("function_definition: could not find function, but it should exist...");
			} else if(existingFunc->symbol==NULL) {
				// check if function definition and prototype are equal (no params)
				checkCompatibleTypesRaw(@1.first_line, $1.type, findFunc(curSymbol,$2)->returnType);
			} else {
				// function was already defined!
				// TODO Dirk create better error functions...
				yyerror("Function already defined!");
			}
		} else {
			insertFunc(curSymbol, newFunc);
		}
	  }
	  BRACE_OPEN {
		  curSymbol = push(curSymbol,findFunc(curSymbol, $2)); 
	  } stmt_list {
		  curSymbol = pop(curSymbol);
	  } BRACE_CLOSE
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		func_t* newFunc = createFunc($2);
		if(newFunc==NULL) {
			error("function_definition: newFunc is NULL");
		}
		newFunc->returnType = $1.type;
		if(exists(curSymbol,$2)) {
			func_t* existingFunc = findFunc(curSymbol, $2);
			if(existingFunc == NULL) {
				error("function_definition: could not find function, but it should exist...");
			} else if(existingFunc->symbol==NULL) {
				// check if function definition and prototype are equal (no params)
				checkCompatibleTypesRaw(@1.first_line, $1.type, findFunc(curSymbol,$2)->returnType);
				correctFuncTypesParam(@1.first_line, curSymbol, $2, $4);
			} else {
				// function was already defined!
				// TODO Dirk create better error functions...
				yyerror("Function already defined!");
			}
		} else {
			insertFunc(curSymbol, newFunc);
		}
	  }	
	  BRACE_OPEN {
		  int ex = exists(curSymbol,$2);
		  curSymbol = push(curSymbol,findFunc(curSymbol, $2));
//		  if(ex) {
//			  // TODO Dirk check if params are correct
//		  } else {
			  insertParams(findFunc(curSymbol, $2),$4);
//		  }
	  } stmt_list {
		  curSymbol = pop(curSymbol);
	  } BRACE_CLOSE
	;
									
/*
 * The non-terminal 'stmt_list' is list of statements containing any number (including zero) of statements represented 
 * by the non-terminal 'stmt'.
 */									
stmt_list
     : /* empty: epsilon */ { }
     | stmt_list stmt { }
     ;

/*
 * The non-terminal 'stmt' is used for the statements of the programming language whereas the non-terminal
 * 'expression' is one of the core statements.
 */									
stmt
     : stmt_block {$$ = newStmt();}
     | variable_declaration SEMICOLON {$$ = newStmt();}
     | expression SEMICOLON {
    	 $$ = newStmt();
    	 if($1->postEmit == PE_FUNCC) {
			emit(NULL,$1,OP_CALL_VOID,NULL);
		}
     }
     | stmt_conditional {$$ = newStmt();}
     | stmt_loop {$$ = newStmt();}
     | RETURN expression SEMICOLON {
    	 $$ = newStmt();
    	 emit($2,NULL,OP_RETURN_VAL,NULL);
    	 // TODO Dirk type checking
     }
     | RETURN SEMICOLON {
    	 $$ = newStmt();
    	 emit(NULL,NULL,OP_RETURN_VAL,NULL);
		 // TODO Dirk type checking
     }
     | SEMICOLON {$$ = newStmt();} /* empty statement */
     ;

/*
 * A statement block is just a statement list within braces.
 */									
stmt_block
     : BRACE_OPEN {} stmt_list BRACE_CLOSE {  /* we could extend additional scopes here :o */}
     ;
	
/*
 * The non-terminal 'stmt_conditional' contains the conditional statements of the language. The second rule
 * produces a SHIFT/REDUCE error which is solved by the default behavior of bison (see above).
 */									
stmt_conditional
     : IF PARA_OPEN expression PARA_CLOSE M stmt {
     	 backpatch($3->trueList, $5.instr);
    	 backpatch($3->falseList, getNextInstr());
     }
     | IF PARA_OPEN expression PARA_CLOSE M stmt ELSE {
    	 $6->nextList = merge($6->nextList, createList(getNextInstr()));
    	 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     } M stmt {
     	 backpatch($3->trueList, $5.instr);
    	 backpatch($3->falseList, $9.instr);
    	 backpatch($6->nextList,getNextInstr());
     }
     ;
									
/*
 * The non-terminal 'stmt_loop' contains the loop statements of the language.
 */									
stmt_loop
     : WHILE PARA_OPEN M expression PARA_CLOSE M stmt {
     	 backpatch($4->trueList, $6.instr);
     	 expr_t* res = newAnonymousExpr();
     	 res->jump = $3.instr;
    	 emit(res, NULL, OP_GOTO, NULL);
    	 backpatch($4->falseList, getNextInstr());
     }
     | DO M stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON {
     	 backpatch($6->trueList, $2.instr);
    	 backpatch($6->falseList, getNextInstr());
     }
     ;
									
/*
 * The non-terminal 'expression' is one of the core statements containing all arithmetic, logical, comparison and
 * assignment operators. 
 */
expression
	: expression ASSIGN expression {
		// check for postEmit expressions
		int normalAssign = 1;
		expr_t* tmpE = NULL;
		if($3->postEmit == PE_FUNCC) {
			if($1->lvalue == 1) {
				// if left expr. is an lValue, we can directly assign to it
				emit($1,$3,OP_CALL_RES,NULL);
			} else {
				// else we need a tmp var
				tmpE = newTmp(T_INT);
				tmpE->type = $1->type;
				emit(tmpE,$3,OP_CALL_RES,NULL);
			}
			normalAssign = 0;
		} else if($3->postEmit == PE_ARR) {
			if($1->lvalue == 1) {
				// if left expr. is an lValue, we can directly assign to it
				emit($1,$3,OP_ARRAY_LD,$3->arrInd);
			} else {
				// else we need a tmp var
				tmpE = newTmp(T_INT);
				tmpE->type = $1->type;
				emit(tmpE,$3,OP_ARRAY_LD,$3->arrInd);
			}
			normalAssign = 0;
		}
		if($1->postEmit == PE_ARR) {
			if(tmpE == NULL) {
				emit($1,$1->arrInd,OP_ARRAY_ST,$3);
			} else {
				emit($1,$1->arrInd,OP_ARRAY_ST,tmpE);
			}
			normalAssign = 0;
		} else {
			if(tmpE != NULL) {
				emit($1,tmpE,OP_ASSIGN,NULL);
			}
		}
		if(normalAssign == 1) {
			int isAssignAllowed = checkCompatibleTypesAssign(@1.first_line, $1, $3);
			if(isAssignAllowed>0 && checkLValue(@1.first_line, $1)) {
				expressionReturn($1);
				emit($1,$3,OP_ASSIGN,NULL);
				$$ = $1;
				if(isAssignAllowed == 2) {
					//$$->
				}
			}
		}
     }
     | expression LOGICAL_OR M expression {
    	 if(checkCompatibleTypes(@1.first_line, $1, $4)) {
    		 expressionReturn($1);
    		 $$ = newAnonymousExpr();
    		 $$->type = $1->type;
			 backpatch($1->falseList, $3.instr);
    		 $$->trueList = merge($1->trueList, $4->trueList);
    		 $$->falseList = $4->falseList;
		 }
	 }
     | expression LOGICAL_AND M expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $4)) {
			 expressionReturn($1);
    		 $$ = newAnonymousExpr();
    		 $$->type = $1->type;
			 backpatch($1->trueList, $3.instr);
    		 $$->trueList = $4->trueList;
    		 $$->falseList = merge($1->falseList, $4->falseList);
		 }
     }
     | LOGICAL_NOT expression { 
    	 $$=$2;
		 indexList_t* tmp = $$->trueList;
    	 $$->trueList = $$->falseList;
		 $$->falseList = tmp;
     }
     | expression EQ expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFEQ,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression NE expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFNE,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression LS expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFLT,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression LSEQ expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFLE,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression GTEQ expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFGE,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression GT expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newAnonymousExpr();
			 $$->falseList = createList(getNextInstr() + 1);
			 $$->trueList = createList(getNextInstr());
			 emit($$,$1,OP_IFGT,$3);
			 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
		 }
     }
     | expression PLUS expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newTmp(T_INT);
			 emit($$,$1,OP_ADD,$3);
			 //printf("l. %d: addition: %d + %d; %d\n", @1.first_line, $1->value, $3->value, $$->type);
		 }
     }
     | expression MINUS expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newTmp(T_INT);
			 emit($$,$1,OP_SUB,$3);
		 }
     }
     | expression MUL expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newTmp(T_INT);
			 emit($$,$1,OP_MUL,$3);
		 }
     }
     | expression DIV expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newTmp(T_INT);
			 emit($$,$1,OP_DIV,$3);
		 }
     }
     | expression MOD expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = newTmp(T_INT);
			 emit($$,$1,OP_MOD,$3);
		 }
     }
     | MINUS expression %prec UNARY_MINUS {
    	 // TODO Dirk type checking
    	 $$ = newTmp(T_INT);
		 emit($$,$2,OP_MINUS,NULL);
     }
     | ID BRACKET_OPEN primary BRACKET_CLOSE {
		 if($3->type!=T_INT) {
			typeError(@1.first_line, "Size of an array has to be of type int, but is of type %s", $1);
		 }
      	 $$ = newExpr($1,T_UNKNOWN);
      	 var_t* found = findVar(curSymbol,$1);
      	 if(found != NULL) {
      		$$->type = found->type;
      	 } else {
      		typeError(@1.first_line, "Array does not exist: %s", $1);
      	 }
  		 $$->arrInd = $3;
  		 $$->postEmit = PE_ARR;
	  }
     | PARA_OPEN expression PARA_CLOSE { 
     	 $$ = $2;
     }
     | function_call { 
    	 $$ = $1;
     }
     | primary { 
		 $$ = $1;
     }
     ;
     
M: /* empty */ { $$.instr = getNextInstr(); };

primary
     : NUM {
    	 $$ = newExprNum($1, T_INT);
    	 $$->lvalue = 0;
       }
     | ID {
    	 $$ = newExpr($1, T_UNKNOWN);
    	 var_t* found = findVar(curSymbol, $1);
    	 if(found!=NULL) {
    		 if(found->type == T_INT_A) {
    			 $$->lvalue = 0;
    		 } else if(found->type == T_INT) {
    			 $$->lvalue = 1;
    		 }
    		 $$->type = found->type;
    	 } else {
    		 typeError(@1.first_line, "Parameter does not exist: %s", $1);
    	 }
      }
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */
function_call
: ID PARA_OPEN PARA_CLOSE {
	$$ = newExpr($1, T_UNKNOWN);
	$$->lvalue = 0;
	func_t* func = findFunc(curSymbol, $1);
	if(func == NULL) {
		// TODO Dirk type checking
	} else {
		$$->type = func->returnType;
	}
	$$->postEmit = PE_FUNCC;
}
| ID PARA_OPEN function_call_parameters PARA_CLOSE { 
	exprList_t* tmp1 = NULL;
	exprList_t* tmp2 = $3;
	GETLISTHEAD(tmp2, tmp1);
	$3 = tmp1;
	correctFuncTypes(@3.first_line, curSymbol,$1,$3); 
	$$ = newExpr($1, T_UNKNOWN);
	$$->lvalue = 0;
	func_t* func = findFunc(curSymbol, $1);
	if(func == NULL) {
		// TODO Dirk type checking
	} else {
		$$->type = func->returnType;
	}
	$$->postEmit = PE_FUNCC;
}
;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
	: function_call_parameters COMMA expression { 
		$$=malloc(sizeof(exprList_t));
		if($$==NULL) {
		 error("fcp_expression: malloc unsuccessful");
		}
		$$->next = NULL;
		$$->expr = $3; 
		$$->prev = $1; 
		$$->prev->next = $$;
		//printf("more than one parameter\n");
	}
	| expression { 
		$$=malloc(sizeof(exprList_t));
		if($$==NULL) {
			error("fcp_expression: malloc unsuccessful");
		}
		$$->expr = $1;
		$$->prev = NULL;
		$$->next = NULL;
		//printf("function call parameters: l. %d: Value: %s \n", @1.first_line, $$->expr->value);
	}
	;

%%

void yyerror (const char *msg)
{
	fprintf(stderr, "Syntax Error: %s",msg);
	exit(1);
} 

