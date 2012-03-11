/* 
 * parser.y - Parser utility for the DHBW compiler!
 */
 
%{
	#include <stdio.h>
	#include <stdarg.h>
	#include <stdlib.h>
	int yylex(void);
	#include "symboltable.h"
%}
 
%debug
%locations
%error-verbose
%start program

%union {
	int num;
	char* str;
}

//%union {
//int integer;
//const char* string;
//}
//%type <integer> NUM
//%type <string> INT
//%type <string> ID

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
%left PLUS MINUS
%left MUL DIV MOD 
%right LOGICAL_NOT NOT UNARY_MINUS UNARY_PLUS
%left BRACKET_OPEN BRACKET_CLOSE


%%

/* 
 * The start symbol of the programming language. According to the
 * original GNU C compiler (GCC), there has to be at least one
 * program_element. The End-of-File token is necessary for completing
 * the parsing process successfully by reducing to the start symbol
 * of grammar 'program'. 
 */
program
     : program_element_list { debug(1); printSymTabKeys(); }
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
     : declaration SEMICOLON { debug(4);}
     | function_definition { debug(5);}
     | SEMICOLON { debug(6);}
     ;
									
/* 
 * The different types of the subset including self-defined data types by using the typedef
 * instruction.
*/
type
     : INT { debug(7);}
     | VOID { debug(8);}
     ;

/* 
 * The non-terminal 'declaration' is used for declarations like
 * 'int a, b, *c[]' as well as function prototypes. 									
/* Each 'declaration' consists of at least one 'declaration_element'. The
 * left-recursion of this rule is positive for the synthesizing the type as 
 * stack-attribute to the 'identifier_declaration'.
*/						
declaration
     : declaration COMMA declaration_element { debug(9);}
     | type declaration_element { debug(10);}
     ;

/*
 * The non-terminal 'declaration_element' contains the different possible elements for 
 * an elementary declaration which could be either a 'function_header' acting as the definition of a function
 * prototype or the declaration of an identifier.
 */
declaration_element
     : identifier_declaration { debug(11);}
     | function_header { debug(12);}
     ;

/*
 * The non-terminal 'identifier_declaration' contains the specifics of the variable beside
 * the type definition like arrays, pointers and initial (default) values.
 */									
identifier_declaration
     : identifier_declaration BRACKET_OPEN expression BRACKET_CLOSE { debug(13);}
     | ID { debug(14);}
     ;

/*
 * The non-terminal 'function_definition' is the beginning of the function definition.
 */									
function_definition
     : type function_header BRACE_OPEN stmt_list BRACE_CLOSE { debug(15);}
     ;

/*
 * The non-terminal 'function_header' is used within the non-terminals 'function' and
 * 'function_prototype'. The grammar for the function definition and the function prototype
 * is split up this way to facilitate the parsing process and to use synthesized attributes
 * during the parsing process.
 */									
function_header
     : function_prefix PARA_CLOSE { debug(16);}
     ;
	
/*
 * The non-terminal 'function_prefix' is used within the non-terminal 'function_header'. The
 * 'function_prefix' distinguishes between functions with parameters by the non-terminal 
 * 'function_signature_parameters' and functions without by the non-terminal 'function_signature'.
 */									
function_prefix
     : function_signature { debug(17);}
     | function_signature_parameters { debug(18);}
     ;

/*
 * The non-terminal 'function_signature' initializes the function signature definition
 */ 									
function_signature
     : identifier_declaration PARA_OPEN { debug(19);}
     ;

/*
 * The non-terminal 'function_signature_parameters' declares the function of the function prototype with
 * (input) parameters.
 */									
function_signature_parameters
     : function_signature_parameters COMMA function_parameter_element { debug(20);}
     | function_signature function_parameter_element { debug(21);}
     ;
	
/*
 * The non-terminal 'function_parameter_element' is used within the non-terminal 'function_definition_parameters'
 * and contains the declaration for ONE parameter.
 */									
function_parameter_element
     : type identifier_declaration { debug(22);}
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
     | declaration SEMICOLON { debug(24);}
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
     : BRACE_OPEN stmt_list BRACE_CLOSE { debug(30);}
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
 * assignment operators.expression 
 */
expression
     : expression ASSIGN expression { debug(35);}
     | expression LOGICAL_OR expression { debug(36);}
     | expression LOGICAL_AND expression { debug(37);}
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
     | expression DIV expression  { debug(48);}
     | expression MOD expression  { debug(49);}
     | MINUS expression %prec UNARY_MINUS { debug(50);}
     | ID BRACKET_OPEN primary BRACKET_CLOSE { debug(51);}
     | PARA_OPEN expression PARA_CLOSE { debug(52);}
     | function_call PARA_CLOSE { debug(53);}
     | primary { debug(54);}
     ;

primary
     : NUM { debug(55); /*printf("<%d>",$1);*/}
     | ID { debug(56); addToSymTab($1);/*printf("<%s>",$1);*/}
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */									
function_call
      : ID PARA_OPEN { debug(57);}
      | function_call_parameters { debug(58);}
      ;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
     : function_call_parameters COMMA expression { debug(59);}
     | ID PARA_OPEN expression { debug(60);}
     ;

%%

void yyerror (const char *msg)
{
	fprintf(stderr,"Syntax Error!\n");
}

