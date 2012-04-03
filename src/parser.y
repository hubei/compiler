/** 
 * @file parser.y 
 * @brief Parser utility for the DHBW compiler!
 */
 
%{
#include <stdio.h>
//#include <stdarg.h>
#include <stdlib.h>
#include "symboltable.h"
#include "typechecking.h"

#define YYERROR_VERBOSE
	
symbol* curSymbol;
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

/**
 * Define types for numericals and strings
 */
%union {
	int num;
	char* str;
	type type;
	var var;
	func func;
	struct {
		type type;
	} expr;
	struct exprList {
		struct expr* expr;
		struct exprList* prev;
		struct exprList* next;
	} exprList;
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

%type <var>identifier_declaration primary function_parameter_list function_parameter
%type <type>type
%type <expr>expression
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
     : {curSymbol = createSymbol()} program_element_list { debug(1); /*printSymTabKeys();*/ }
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
     : INT { debug(7); $$ = T_INT; }
     | VOID { debug(8); $$ = T_VOID;  }
     ;

/*
 * 
 */
variable_declaration
	: variable_declaration COMMA identifier_declaration	 {debug(48); }
	| type identifier_declaration {
		debug(42);
		$2.type = $1;
		insertVar(curSymbol, &$2);
	}
	;

/*
 * The non-terminal 'identifier_declaration' contains the specifics of the variable beside
 * the type definition like arrays, pointers and initial (default) values.
 */									
identifier_declaration
     : identifier_declaration BRACKET_OPEN NUM BRACKET_CLOSE { /* BRACKET = [] !!! */
    	 debug(13); 
    	 $$ = $1;
		 $$.size *= $3;
// example of how to get line numbers
		fprintf (stderr, "l%d,c%d-l%d,c%d\n",
                            @1.first_line, @1.first_column,
                            @1.last_line, @1.last_column);
     } 
     | ID {	
    	 debug(14); 
    	 $$.id = $1;
    	 $$.size=1; 
     }
     ;
 	
 /*
 * 
 */
function_declaration
	: type ID PARA_OPEN PARA_CLOSE {
		debug(47); 
		func* func = createFunc();
		func->id = $2;
		func->returnType = $1;
		func->param = NULL;
		insertFunc(curSymbol, func);
	}
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
		debug(44);
		func* func = createFunc();
		func->id = $2;
		func->returnType = $1;
		func->param = &$4;
		insertFunc(curSymbol, func);
	}
	;

/*
 * 
 */
function_parameter_list
	: function_parameter {
		debug(45); 
		$$ = $1;
	}
	| function_parameter_list COMMA function_parameter {
		debug(46);
		addToVar(&$$, &$3);
	}
	;

/*
 * 
 */
function_parameter
	: type identifier_declaration {
		debug(43);
		$2.type = $1;
		$$ = $2;
	}
	;
	
/*
 * The non-terminal 'function_definition' is the beginning of the function definition.
 */
function_definition
	: type ID PARA_OPEN PARA_CLOSE {
		/* typechecking */
	  }
	  BRACE_OPEN {curSymbol = push(curSymbol);} stmt_list {curSymbol = pop(curSymbol);} BRACE_CLOSE
	| type ID PARA_OPEN function_parameter_list PARA_CLOSE {
			/* typechecking (returntype, parameters) */
		}	
	  BRACE_OPEN {curSymbol = push(curSymbol);} stmt_list {curSymbol = pop(curSymbol);} BRACE_CLOSE
	;
									
/*
 * The non-terminal 'stmt_list' is list of statements containing any number (including zero) of statements represented 
 * by the non-terminal 'stmt'.
 */									
stmt_list
     : /* empty: epsilon */
     | stmt_list stmt { debug(23);}
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
     : BRACE_OPEN stmt_list BRACE_CLOSE { debug(30); /* we could extend additional scopes here :o */}
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
     : expression ASSIGN expression { debug(35); }
     | expression LOGICAL_OR expression { 
    	 debug(36);
     }
     | expression LOGICAL_AND expression { 
    	 debug(37);
	 }
     | LOGICAL_NOT expression { debug(38);}
     | expression EQ expression { debug(39);}
     | expression NE expression { debug(40);}
     | expression LS expression  { debug(41);}
     | expression LSEQ expression  { debug(42);}
     | expression GTEQ expression  { debug(43);}
     | expression GT expression { debug(44);}
     | expression PLUS expression { debug(45);}
     | expression MINUS expression { debug(46);}
     | expression MUL expression { debug(47);}
     | expression DIV expression  { 
		debug(48);
		}
     | expression MOD expression  { debug(49);}
     | MINUS expression %prec UNARY_MINUS { debug(50);}
     | ID BRACKET_OPEN primary BRACKET_CLOSE { debug(51);}
     | PARA_OPEN expression PARA_CLOSE { debug(52);}
     | function_call { debug(53);}
     | primary { debug(54); }
     ;

primary
     : NUM { debug(55); $$.type = T_INT; }
     | ID { debug(56); $$ = *findVar(curSymbol, $1); }
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */
function_call
	: ID PARA_OPEN PARA_CLOSE { debug(57); /* typechecking */}
	| ID PARA_OPEN function_call_parameters PARA_CLOSE { debug(58); if (!correctFuncTypes($1,$3)) {yyerror("Wrong parameter in function call");}}
	;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
     : function_call_parameters COMMA expression { debug(59); $$.expr = &$3; $$.prev = &$1; $$.prev->next = &$$; /*FIXME maybe check for nullpointers? :P*/}
     | expression { debug(60); $$.expr = &$1; }
     ;

%%

void yyerror (const char *msg)
{
	fprintf(stderr,"Syntax Error!\n");
}

