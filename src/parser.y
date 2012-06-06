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
#include <errno.h>



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
%type <expr>expression function_call primary stmt_conditional_start
%type <typeExt>type variable_declaration
%type <exprList>function_call_parameters
%type <statmt>stmt stmt_conditional stmt_loop stmt_block stmt_list
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
 * The non-terminal 'variable_declaration' contains the type of a variable and an
 * (optionally comma separted list of) identifier_declaration 
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
		assert($2 != NULL);
		if($2->type == T_INT_A && $1.type!=T_INT) {
			typeError(@1.first_line, "The syntax of %s cannot be used to declare %s. Only int is possible.",$2->id, typeToString($1.type));
		}
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
	: ID BRACKET_OPEN NUM BRACKET_CLOSE { // array
		if(exists(curSymbol, $1)) {
			var_t* var = findVar(curSymbol, $1);
			if(var == NULL) {
				errorIdDeclared(@1.first_line, $1);
			} else {
				errorVarDeclared(@1.first_line, $1);
			}
		} else {
			var_t* newVar = createVar($1);
			if(newVar == NULL) {
				// TODO error memory
			}
			$$ = newVar;
			$$->type = T_INT_A;
			$$->size = $3;
		}
		free($1);
	} 
	| ID {	
		if(exists(curSymbol, $1)) {
			var_t* var = findVar(curSymbol, $1);
			if(var == NULL) {
				errorIdDeclared(@1.first_line, $1);
			} else {
				errorVarDeclared(@1.first_line, $1);
			}
		} else {
			$$ = createVar($1);
			$$->size=0; 
		}
		free($1);
	}
	;
 	
 /*
 * 
 */
function_declaration
	: type ID PARA_OPEN PARA_CLOSE {
		if(exists(curSymbol, $2)) {
			func_t* func = findFunc(curSymbol, $2);
			if(func == NULL) {
				errorIdDeclared(@2.first_line, $2);
			} else {
				errorFuncDeclared(@2.first_line, $2);
			}
		} else {
			func_t* newFunc = createFunc($2);
			newFunc->returnType = $1.type;
			newFunc->param = NULL;
			insertFunc(curSymbol, newFunc);
		}
		free($2);
	}
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		if(exists(curSymbol, $2)) {
			func_t* func = findFunc(curSymbol, $2);
			if(func == NULL) {
				errorIdDeclared(@2.first_line, $2);
			} else {
				errorFuncDeclared(@2.first_line, $2);
			}
		} else {
			func_t* newFunc = createFunc($2);
			newFunc->returnType = $1.type;
			GETLISTHEAD($4, newFunc->param);
			newFunc->num_params = getParamCount(newFunc->param);
			insertFunc(curSymbol, newFunc);
		}
		free($2);
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
		assert($2 != NULL);
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
		if(exists(curSymbol,$2)) {
			func_t* existingFunc = findFunc(curSymbol, $2);
			if(existingFunc == NULL) {
				errorIdDeclared(@2.first_line, $2);
			} else if(existingFunc->symbol==NULL) {
				// check if function definition and prototype are equal (no params)
				checkCompatibleTypesRaw(@2.first_line, $1.type, existingFunc->returnType);
			} else {
				// function was already defined!
				errorFuncDeclared(@2.first_line, $2);
			}
		} else {
			func_t* newFunc = createFunc($2);
			newFunc->returnType = $1.type;
			newFunc->param = NULL;
			insertFunc(curSymbol, newFunc);
		}
	  }
	  BRACE_OPEN {
		  curSymbol = push(curSymbol,findFunc(curSymbol, $2)); 
	  } stmt_list {
		  curSymbol = pop(curSymbol);
		  checkReturnTypes(@1.first_line, $1.type, $8->returnType);
		  clean_stmt($8);
		  free($2);
	  } BRACE_CLOSE
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		if(exists(curSymbol,$2)) {
			func_t* existingFunc = findFunc(curSymbol, $2);
			if(existingFunc == NULL) {
				errorIdDeclared(@2.first_line, $2);
			} else if(existingFunc->symbol==NULL) {
				// check if function definition and prototype are equal (with params)
				param_t* head;
				GETLISTHEAD($4, head);
				checkCompatibleTypesRaw(@1.first_line, $1.type, existingFunc->returnType);
				correctFuncTypesParam(@4.first_line, curSymbol, $2, head);
			} else {
				// function was already defined!
				errorFuncDeclared(@2.first_line, $2);
			}
		} else {
			func_t* newFunc = createFunc($2);
			newFunc->returnType = $1.type;
			insertFunc(curSymbol, newFunc);
		}
	  }	
	  BRACE_OPEN {
		  int ex = exists(curSymbol,$2);
		  curSymbol = push(curSymbol,findFunc(curSymbol, $2));
		  insertParams(findFunc(curSymbol, $2),$4);
	  } stmt_list {
		  curSymbol = pop(curSymbol);
		  checkReturnTypes(@1.first_line, $1.type, $9->returnType);
		  clean_stmt($9);
		  free($2);
	  } BRACE_CLOSE
	;
									
/*
 * The non-terminal 'stmt_list' is a list of statements containing any number (including zero) of statements represented 
 * by the non-terminal 'stmt'.
 */									
stmt_list
     : /* empty: epsilon */ { $$ = newStmt(); }
     | stmt_list stmt {
    	 if($1->returnType!=$2->returnType &&
    			 $1->returnType!=T_UNKNOWN &&
    			 $2->returnType!=T_UNKNOWN) {
    		 typeError(@2.first_line, "Two differnt return types (%s, %s) in function ",$1, $2);
    	 } else {
    		 if ($2->returnType==T_UNKNOWN) {
    			 $$ = $1;
         	 	 clean_stmt($2);
    		 }
    		 else {
    			 $$ = $2;
         	 	 clean_stmt($1);
    		 }
    	 }
     }
     ;

/*
 * The non-terminal 'stmt' is used for the statements of the programming language whereas the non-terminal
 * 'expression' is one of the core statements.
 */									
stmt
     : stmt_block {$$ = $1;}
     | variable_declaration SEMICOLON {$$ = newStmt();}
     | expression SEMICOLON {
    	 $$ = newStmt();
    	 if($1->postEmit == PE_FUNCC) {
    		 delLastInstr();
    		 expr_t* tmp = newExpr($1->parentId,T_UNKNOWN);
    		 tmp->params = $1->params;
    		 
			 emit(NULL,tmp,OP_CALL_VOID,NULL);
		}
     }
     | stmt_conditional {$$ = newStmt();}
     | stmt_loop {
    	 $$ = newStmt();
     }
     | RETURN expression SEMICOLON {
    	 $$ = newStmt();
    	 emit($2,NULL,OP_RETURN_VAL,NULL);
    	 $$->returnType = $2->type;
     }
     | RETURN SEMICOLON {
    	 $$ = newStmt();
    	 emit(NULL,NULL,OP_RETURN_VAL,NULL);
    	 $$->returnType = T_VOID;
     }
     | SEMICOLON { 
    	 $$ = newStmt(); 
    	 /* empty statement */
     }
     ;

/*
 * A statement block is just a statement list within braces.
 */									
stmt_block
     : BRACE_OPEN stmt_list BRACE_CLOSE {  $$ = $2; /* we could extend additional scopes here :o */}
     ;
	
/*
 * The non-terminal 'stmt_conditional' contains the conditional statements of the language. The second rule
 * produces a SHIFT/REDUCE error which is solved by the default behavior of bison (see above).
 */									
     stmt_conditional_start
          : IF PARA_OPEN expression {
        	  //TODO review: compound added, default = 0, set to 1 if two expressions are compounded
         	 if($3->compound == 1) { 
         		 delLastInstr();
         		 emit($3, $3,OP_IFGT,newExprNum(0, T_INT)); //TODO what is "res"? => result, but depending on OP; for if, it is only used for the GOTO number
         	 }
         	 $$ = $3;
          };
          
	  stmt_conditional
          : stmt_conditional_start PARA_CLOSE M stmt {
           	 backpatch($1->trueList, $3.instr);
          	 backpatch($1->falseList, getNextInstr());
          	 clean_stmt($4);
           }
          | stmt_conditional_start PARA_CLOSE M stmt ELSE {
          	 $4->nextList = merge($4->nextList, newIndexList(getNextInstr()));
          	 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
           } M stmt {
           	 backpatch($1->trueList, $3.instr);
          	 backpatch($1->falseList, $7.instr);
          	 backpatch($4->nextList,getNextInstr());
          	 clean_stmt($4);
          	 clean_stmt($8);
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
    	 clean_stmt($7);
     }
     | DO M stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON {
     	 backpatch($6->trueList, $2.instr);
    	 backpatch($6->falseList, getNextInstr());
    	 clean_stmt($3);
     }
     ;
									
/*
 * The non-terminal 'expression' is one of the core statements containing all arithmetic, logical, comparison and
 * assignment operators. 
 */
expression
	: expression ASSIGN {if($1->postEmit == PE_ARR) delLastInstr();} expression {
		$$ = NULL;

		
		
		// check for postEmit expressions
		int normalAssign = 1;
		expr_t* tmpE = NULL;
		if($4->postEmit == PE_ARR) {
			// we do not need the last instruction
			delLastInstr();

			if($1->lvalue == 1) {
				// if left expr. is an lValue, we can directly assign to it
				// x = arr[i]
				expr_t *newEx = newExpr($4->parentId,T_INT);
				checkCompatibleTypes(@1.first_line, $1, newEx);
				emit($1,newEx,OP_ARRAY_LD,$4->arrInd);
			} else {
				// else we need a tmp var
				// tmp = arr[i]
				tmpE = newTmp($1->type);
				expr_t *newEx = newExpr($4->parentId,T_INT);
				checkCompatibleTypes(@1.first_line, tmpE, newEx);
				emit(tmpE,newEx,OP_ARRAY_LD,$4->arrInd);
			}
			normalAssign = 0;
		}
		if($1->postEmit == PE_ARR) {
			if(tmpE == NULL) {
				// arr[i] = $4
				expr_t *newEx = newExpr($1->parentId,T_INT);
				checkCompatibleTypes(@1.first_line, $4, newEx);
				emit(newEx,$1->arrInd,OP_ARRAY_ST,$4);
				$$ = $4;
			} else {
				// arr[i] = tmpE
				expr_t *newEx = newExpr($1->parentId,T_INT);
				checkCompatibleTypes(@1.first_line, tmpE, newEx);
				emit(newEx,$1->arrInd,OP_ARRAY_ST,tmpE);
				$$ = tmpE;
			}
			normalAssign = 0;
		} else {
			if(tmpE != NULL) {
				// if a temp expr was needed, assign it to $1
				emit($1,tmpE,OP_ASSIGN,NULL);
			}
		}
		if(normalAssign == 1) {
			int isAssignAllowed = checkCompatibleTypesAssign(@1.first_line, $1, $4);
			//types are compatible and the left value is an assignable expression
			if(isAssignAllowed>0 && checkLValue(@1.first_line, $1)) {
				if($4->falseList != NULL || $4->trueList != NULL) {
					$$ = newTmp(T_INT);
					backpatch($4->trueList, getNextInstr());
					emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
					emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
					backpatch($4->falseList, getNextInstr());
					
					emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
					backpatch(newIndexList(getNextInstr() - 2), getNextInstr());
					emit($1,$$,OP_ASSIGN,NULL);
				} else {
					emit($1,$4,OP_ASSIGN,NULL);
					$$ = $1;
				}
			}
		}
		if($$ == NULL) {
			$$ = $1;
		}
     }
     | expression LOGICAL_OR M expression {
    	 checkCompatibleTypes(@1.first_line, $1, $4);
		 $$ = newAnonymousExpr();
		 $$->type = $1->type;
		 $$->compound = 1;
		 backpatch($1->falseList, $3.instr);
		 $$->trueList = merge($1->trueList, $4->trueList);
		 $$->falseList = $4->falseList;
	 }
     | expression LOGICAL_AND M expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $4);
//		 $$ = newTmp(T_INT);
    	 $$ = $4;
		 $$->type = $1->type;
		 $$->compound = 1;
		 backpatch($1->trueList, $3.instr);
		 $$->trueList = $4->trueList;
		 $$->falseList = merge($1->falseList, $4->falseList);
		 // tmp = 1
//		 emit($$,newExprNum(2,T_INT), OP_ASSIGN,NULL);
     }
     | LOGICAL_NOT expression { 
    	 $$=$2;
		 indexList_t* tmp = $$->trueList;
    	 $$->trueList = $$->falseList;
		 $$->falseList = tmp;
     }
     | expression EQ expression {
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFEQ,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression NE expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFNE,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression LS expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFLT,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression LSEQ expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFLE,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression GTEQ expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFGE,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression GT expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 $$->falseList = newIndexList(getNextInstr() + 1);
		 $$->trueList = newIndexList(getNextInstr());
		 // tmp = 1
//		 emit($$,newExprNum(1,T_INT), OP_ASSIGN,NULL);
		 // if $1 OP $3 GOTO
		 emit($$,$1,OP_IFGT,$3);
		 // tmp = 0
//		 emit($$,newExprNum(0,T_INT), OP_ASSIGN,NULL);
		 // GOTO
		 emit(newAnonymousExpr(), NULL, OP_GOTO, NULL);
     }
     | expression PLUS expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_ADD,$3);
     }
     | expression MINUS expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_SUB,$3);
     }
     | expression MUL expression { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_MUL,$3);
     }
     | expression DIV expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_DIV,$3);
     }
     | expression MOD expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_MOD,$3);
     }
     | expression SHIFT_LEFT expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_MOD,$3);
     }
     | expression SHIFT_RIGHT expression  { 
    	 checkCompatibleTypes(@1.first_line, $1, $3);
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 emit($$,$1,OP_MOD,$3);
     }
     | MINUS expression %prec UNARY_MINUS {
    	 checkCompatibleTypesRaw(@1.first_line, $2->type, T_INT);
    	 $$ = newTmp(T_INT);
    	 $$->compound = 1;
		 emit($$,$2,OP_MINUS,NULL);
     }
     | ID BRACKET_OPEN primary BRACKET_CLOSE {
		 $$ = newTmp(T_INT);
		 $$->compound = 1;
		 // check type
		 if($3->type!=T_INT) {
			typeError(@1.first_line, "Size of an array has to be of type int, but is of type %s", $1);
		 }
		 // check for existing
      	 var_t* found = findVar(curSymbol,$1);
      	 if(found == NULL) {
      		typeError(@1.first_line, "Array does not exist: %s", $1);
      		$$->parentId = "unknown";
      	 } else {
    		 $$->parentId = found->id;
      	 }
		 $$->arrInd = $3;
		 $$->postEmit = PE_ARR;
		 expr_t* tmpE = newExpr($1,T_INT);
		 emit($$,tmpE,OP_ARRAY_LD,$$->arrInd);
  		 
		 free($1);
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

/*
 * The non-terminal 'M' is used as a marker for instructions
 */
M: /* empty */ { $$.instr = getNextInstr(); };

/*
 * The non-terminal 'primary' is used by the non-terminal 'expression' for parsing an id or a number
 */
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
 		free($1);
      }
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */
function_call
: ID PARA_OPEN PARA_CLOSE {
	func_t* func = findFunc(curSymbol, $1);
	if(func == NULL) {
		errorFuncCall(@1.first_line, $1);
	} else {
		correctFuncTypes(@1.first_line, curSymbol,func->id,NULL);
		$$ = newTmp(T_INT);
		$$->type = func->returnType;
		$$->lvalue = 0;
		$$->postEmit = PE_FUNCC;
		$$->parentId = func->id;	
		emit($$,newExpr(func->id, T_UNKNOWN),OP_CALL_RES,NULL);
	}

	free($1);
}
| ID PARA_OPEN function_call_parameters PARA_CLOSE { 
	func_t* func = findFunc(curSymbol, $1);
	if(func == NULL) {
		errorFuncCall(@1.first_line, $1);
	} else {
		// set $3 to HEAD
		exprList_t* tmp1 = NULL;
		GETLISTHEAD($3, tmp1);
		// check types
		correctFuncTypes(@3.first_line, curSymbol,func->id,tmp1);
		// create new tmp for function call
		$$ = newTmp(T_INT);
		$$->lvalue = 0;
		$$->params = tmp1;
		$$->postEmit = PE_FUNCC;
		$$->parentId = func->id;
		$$->type = func->returnType;
		
		emit($$,newExpr(func->id, T_UNKNOWN),OP_CALL_RES,NULL);
	}
	
	free($1);
}
;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
	: expression COMMA function_call_parameters { 
		$$=newExprList($1);
		$$->next = $3;
		$$->expr = $1; 
		$$->next->prev = $$;
	}
	| expression { 
		$$=newExprList($1);
	}
	;

%%

void yyerror (const char *msg)
{
	fprintf(stderr, "%s\n",msg);
	exit(42);
} 

