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
	
symbol_t* curSymbol;
	
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

%%

/* 
 * The start symbol of the programming language. According to the
 * original GNU C compiler (GCC), there has to be at least one
 * program_element. The End-of-File token is necessary for completing
 * the parsing process successfully by reducing to the start symbol
 * of grammar 'program'. 
 */
program
     : {curSymbol = getSymbolTable();} program_element_list { debug(1); }
     ;

/*
 * The list of program elements which are considered as high-level elements like function
 * definitions, global (constant) variables. The non-terminal 'program_element_list' consists at 
 * least of one program element. Though, empty source files will not succeed.
 */									
program_element_list
     : program_element_list program_element  { debug(2);}
     | program_element { debug(3);}
     ;

/*
 * The different program elements which are variable declarations, function definitions and
 * function prototypes and type definitions for the basic version of the compiler. 
 */									
program_element
     : variable_declaration SEMICOLON { debug(4);}
     | function_declaration SEMICOLON { debug(49);}
     | function_definition { debug(5);}
     | SEMICOLON { debug(6);}
     ;
									
/* 
 * The different types of the subset including self-defined data types by using the typedef
 * instruction.
*/
type
     : INT { debug(7); $$.type = T_INT; $$.width = 4; }
     | VOID { debug(8); $$.type = T_VOID; $$.width = 0; }
     ;

/*
 * 
 */
variable_declaration
	: variable_declaration COMMA identifier_declaration	 {
		debug(100);
		$3->type = $1.type;
		$3->width = $1.width;
		if($3->size != 0) {
			$3->type = T_INT_A;
			$3->width = $3->size * $1.width;
		}
		insertVar(curSymbol, $3);
	}
	| type identifier_declaration {
		debug(42);
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
    	 debug(13); 
    	 var_t* newVar = createVar($1);
    	 if(newVar == NULL) {
    		 error("identifier_declaration: newVar is NULL");
    	 }
    	 $$ = newVar;
		 $$->size = $3;
// example of how to get line numbers
//		fprintf (stderr, "l%d,c%d-l%d,c%d\n",
//                            @1.first_line, @1.first_column,
//                            @1.last_line, @1.last_column);
     } 
     | ID {	
    	 debug(14);
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
		debug(47); 
		func_t* newFunc = createFunc($2);
		if(newFunc==NULL) {
			error("function_declaration: newFunc is NULL");
		}
		newFunc->returnType = $1.type;
		newFunc->param = NULL;
		insertFunc(curSymbol, newFunc);
	}
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		debug(44);
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
		debug(45);
		$$ = addParam(NULL, $1);
	}
	| function_parameter_list COMMA function_parameter {
		debug(46);
		$$ = addParam($1, $3);
	}
	;

/*
 * 
 */
function_parameter
	: type identifier_declaration {
		debug(43);
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
			checkCompatibleTypesRaw(@1.first_line, $1.type, findFunc(curSymbol,$2)->returnType);
			// TODO Dirk RESOLVED check if function definition and prototype are equal (no params)
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
				checkCompatibleTypesRaw(@1.first_line, $1.type, findFunc(curSymbol,$2)->returnType);
				correctFuncTypesParam(@1.first_line, curSymbol, $2, $4);
				// TODO Dirk RESOLVED check if function definition and prototype are equal (with params)
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
     : /* empty: epsilon */ { debug(120);}
     | stmt_list { debug(121);} stmt { debug(23);}
     ;

/*
 * The non-terminal 'stmt' is used for the statements of the programming language whereas the non-terminal
 * 'expression' is one of the core statements.
 */									
stmt
     : stmt_block
     | variable_declaration SEMICOLON { debug(24);}
     | expression SEMICOLON { debug(25);}
     | stmt_conditional { debug(26);}
     | stmt_loop { debug(27);}
     | RETURN expression SEMICOLON { debug(28);}
     | RETURN SEMICOLON { debug(29);}
     | SEMICOLON /* empty statement */
     ;

/*
 * A statement block is just a statement list within braces.
 */									
stmt_block
     : BRACE_OPEN {debug(110);} stmt_list BRACE_CLOSE { debug(30); /* we could extend additional scopes here :o */}
     ;
	
/*
 * The non-terminal 'stmt_conditional' contains the conditional statements of the language. The second rule
 * produces a SHIFT/REDUCE error which is solved by the default behavior of bison (see above).
 */									
stmt_conditional
     : IF PARA_OPEN expression PARA_CLOSE stmt { debug(31);}
     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE stmt { debug(32);}
     ;
									
/*
 * The non-terminal 'stmt_loop' contains the loop statements of the language.
 */									
stmt_loop
     : WHILE PARA_OPEN expression PARA_CLOSE stmt { debug(33);}
     | DO stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON { debug(34);}
     ;
									
/*
 * The non-terminal 'expression' is one of the core statements containing all arithmetic, logical, comparison and
 * assignment operators. 
 */
expression
     : expression ASSIGN expression {
     	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
     		expressionReturn($1);
     		$$ = $1;
     		createIRCodeFromExpr(curSymbol,$1,OP_ASSIGN,$3);
     	 }
     }
     | expression LOGICAL_OR expression {
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
    		 expressionReturn($1);
    		 $$ = $1;
		 }
	 }
     | expression LOGICAL_AND expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
		 }
     }
     | LOGICAL_NOT expression { debug(38); $$=$2;}
     | expression EQ expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFEQ,$3);
		 }
     }
     | expression NE expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFNE,$3);
		 }
     }
     | expression LS expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFLT,$3);
		 }
     }
     | expression LSEQ expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFLE,$3);
		 }
     }
     | expression GTEQ expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFGE,$3);
		 }
     }
     | expression GT expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_IFGT,$3);
		 }
     }
     | expression PLUS expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_ADD,$3);
		 }
     }
     | expression MINUS expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_SUB,$3);
		 }
     }
     | expression MUL expression { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_MUL,$3);
		 }
     }
     | expression DIV expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_DIV,$3);
		 }
     }
     | expression MOD expression  { 
    	 if(checkCompatibleTypes(@1.first_line, $1, $3)) {
			 expressionReturn($1);
			 $$ = $1;
			 createIRCodeFromExpr(curSymbol,$1,OP_MOD,$3);
		 }
     }
     | MINUS expression %prec UNARY_MINUS { debug(50); $$ = $2;}
     | ID BRACKET_OPEN primary BRACKET_CLOSE { debug(51); 
     	 if($3->type!=T_INT) {
     		typeError(@1.first_line, "Size of an array has to be of type int, but is of type %s", $1);
     	 }
     	 $$=$3;
     	 $$->type=T_INT;
     	 $$->lvalue=1;
     }
     | PARA_OPEN expression PARA_CLOSE { debug(52); $$ = $2;}
     | function_call { debug(53); $$ = $1;}
     | primary { debug(54); $$ = $1;}
     ;

primary
     : NUM { 
    	debug(55);
//		if($$==NULL) {
//			error("primary: $1 is NULL");
//		}
    	$$=malloc(sizeof(*$$));
    		 $$->value.num = $1;
    	 	 $$->type = T_INT;
    	 	 $$->lvalue = 0;
    	 	 $$->valueKind = VAL_NUM;
       }
     | ID { 
    	 debug(56); 
    	 var_t* found = findVar(curSymbol, $1);
    	 if(found!=NULL) {
    		 if(found->type == T_INT_A) {
    			 $$->lvalue = 0;
    		 } else if(found->type == T_INT) {
    			 $$->lvalue = 1;
    		 }
    		 $$->value.id = $1;
    		 $$->type = found->type;
    		 //printf("%d: Type: %d \n", @1.first_line, $$.type);
    		 $$->valueKind = VAL_ID;
    	 } else {
    		 typeError(@1.first_line, "Parameter does not exist: %s", $1);
    	 }
      }
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */
function_call
	: ID PARA_OPEN PARA_CLOSE { debug(57); }
	| ID PARA_OPEN function_call_parameters PARA_CLOSE { 
		debug(58); 
		printf("fucntion call: %d: Value: %s \n", @1.first_line, $3->expr->value);
		correctFuncTypes(@3.first_line, curSymbol,$1,$3); 
	}
	;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
     : function_call_parameters COMMA expression { debug(59); 
//     	 $$->expr = &$3; 
//     	 $$->prev = &$1; 
//     	 $$->prev->next = $$;
//     	 exprList_t* tmp1 = NULL;
//     	 exprList_t* tmp2 = $$;
//     	 //GETLISTHEAD(tmp2, tmp1);
//     	 $$ =  tmp1;
     	 printf("more than one parameter\n");
     	 /*FIXME maybe check for nullpointers? :P*/}
     | expression { 
    	 $$=malloc(sizeof(*$$));
    	 assert($$!=NULL);
    	 $$->expr = $1;
    	 printf("fucntion call parameters: %d: Value: %s \n", @1.first_line, $$->expr->value);
     }
     ;

%%

void yyerror (const char *msg)
{
	fprintf(stderr, "Syntax Error: %s",msg);
	//exit(1);
} 

