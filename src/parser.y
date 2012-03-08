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
     : program_element_list { add(1);}
     ;

/*
 * The list of program elements which are considered as high-level elements like function
 * definitions, global (constant) variables. The non-terminal 'program_element_list' consists at 
 * least of one program element. Though, empty source files will not succeed.
 */									
program_element_list
     : program_element_list program_element  { add(2);}
     | program_element { add(3);}
     ;

/*
 * The different program elements which are variable declarations, function definitions and
 * function prototypes and type definitions for the basic version of the compiler. 
 */									
program_element
     : declaration SEMICOLON { add(4);}
     | function_definition { add(5);}
     | SEMICOLON { add(6);}
     ;
									
/* 
 * The different types of the subset including self-defined data types by using the typedef
 * instruction.
*/
type
     : INT { add(7);}
     | VOID { add(8);}
     ;

/* 
 * The non-terminal 'declaration' is used for declarations like
 * 'int a, b, *c[]' as well as function prototypes. 									
/* Each 'declaration' consists of at least one 'declaration_element'. The
 * left-recursion of this rule is positive for the synthesizing the type as 
 * stack-attribute to the 'identifier_declaration'.
*/						
declaration
     : declaration COMMA declaration_element { add(9);}
     | type declaration_element { add(10);}
     ;

/*
 * The non-terminal 'declaration_element' contains the different possible elements for 
 * an elementary declaration which could be either a 'function_header' acting as the definition of a function
 * prototype or the declaration of an identifier.
 */
declaration_element
     : identifier_declaration { add(11);}
     | function_header { add(12);}
     ;

/*
 * The non-terminal 'identifier_declaration' contains the specifics of the variable beside
 * the type definition like arrays, pointers and initial (default) values.
 */									
identifier_declaration
     : identifier_declaration BRACKET_OPEN expression BRACKET_CLOSE { add(13);}
     | ID { add(14);}
     ;

/*
 * The non-terminal 'function_definition' is the beginning of the function definition.
 */									
function_definition
     : type function_header BRACE_OPEN stmt_list BRACE_CLOSE { add(15);}
     ;

/*
 * The non-terminal 'function_header' is used within the non-terminals 'function' and
 * 'function_prototype'. The grammar for the function definition and the function prototype
 * is split up this way to facilitate the parsing process and to use synthesized attributes
 * during the parsing process.
 */									
function_header
     : function_prefix PARA_CLOSE { add(16);}
     ;
	
/*
 * The non-terminal 'function_prefix' is used within the non-terminal 'function_header'. The
 * 'function_prefix' distinguishes between functions with parameters by the non-terminal 
 * 'function_signature_parameters' and functions without by the non-terminal 'function_signature'.
 */									
function_prefix
     : function_signature { add(17);}
     | function_signature_parameters { add(18);}
     ;

/*
 * The non-terminal 'function_signature' initializes the function signature definition
 */ 									
function_signature
     : identifier_declaration PARA_OPEN { add(19);}
     ;

/*
 * The non-terminal 'function_signature_parameters' declares the function of the function prototype with
 * (input) parameters.
 */									
function_signature_parameters
     : function_signature_parameters COMMA function_parameter_element { add(20);}
     | function_signature function_parameter_element { add(21);}
     ;
	
/*
 * The non-terminal 'function_parameter_element' is used within the non-terminal 'function_definition_parameters'
 * and contains the declaration for ONE parameter.
 */									
function_parameter_element
     : type identifier_declaration { add(22);}
     ;
									
/*
 * The non-terminal 'stmt_list' is list of statements containing any number (including zero) of statements represented 
 * by the non-terminal 'stmt'.
 */									
stmt_list
     : /* empty: epsilon */
     | stmt_list stmt { add(23);}
     ;

/*
 * The non-terminal 'stmt' is used for the statements of the programming language whereas the non-terminal
 * 'expression' is one of the core statements.
 */									
stmt
     : stmt_block
     | declaration SEMICOLON { add(24);}
     | expression SEMICOLON { add(25);}
     | stmt_conditional { add(26);}
     | stmt_loop { add(27);}
     | RETURN expression SEMICOLON { add(28);}
     | RETURN SEMICOLON { add(29);}
     | SEMICOLON /* empty statement */
     ;

/*
 * A statement block is just a statement list within braces.
 */									
stmt_block
     : BRACE_OPEN stmt_list BRACE_CLOSE { add(30);}
     ;
	
/*
 * The non-terminal 'stmt_conditional' contains the conditional statements of the language. The second rule
 * produces a SHIFT/REDUCE error which is solved by the default behavior of bison (see above).
 */									
stmt_conditional
     : IF PARA_OPEN expression PARA_CLOSE stmt { add(31);}
     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE stmt { add(32);}
     ;
									
/*
 * The non-terminal 'stmt_loop' contains the loop statements of the language.
 */									
stmt_loop
     : WHILE PARA_OPEN expression PARA_CLOSE stmt { add(33);}
     | DO stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON { add(34);}
     ;
									
/*
 * The non-terminal 'expression' is one of the core statements containing all arithmetic, logical, comparison and
 * assignment operators.expression 
 */									
expression
     : expression ASSIGN expression { add(35);}
     | expression LOGICAL_OR expression { add(36);}
     | expression LOGICAL_AND expression { add(37);}
     | LOGICAL_NOT expression { add(38);}
     | expression EQ expression { add(39);}
     | expression NE expression { add(40);}
     | expression LS expression  { add(41);}
     | expression LSEQ expression  { add(42);}
     | expression GTEQ expression  { add(43);}
     | expression GT expression { add(44);}
     | expression PLUS expression { add(45);}
     | expression MINUS expression { add(46);}
     | expression MUL expression { add(47);}
     | expression DIV expression  { add(48);}
     | expression MOD expression  { add(49);}
     | MINUS expression %prec UNARY_MINUS { add(50);}
     | ID BRACKET_OPEN primary BRACKET_CLOSE { add(51);}
     | PARA_OPEN expression PARA_CLOSE { add(52);}
     | function_call PARA_CLOSE { add(53);}
     | primary { add(54);}
     ;

primary
     : NUM { add(55); printf("<%d>",$1);}
     | ID { add(56); printf("<%s>",$1);}
     ;

/*
 * The non-terminal 'function_call' is used by the non-terminal 'expression' for calling functions.
 */									
function_call
      : ID PARA_OPEN { add(57);}
      | function_call_parameters { add(58);}
      ;

/*
 * The non-terminal 'function_call_parameters' is used for the parameters of a function call 
 * by the non-terminal 'function_call'.
 */ 									
function_call_parameters
     : function_call_parameters COMMA expression { add(59);}
     | ID PARA_OPEN expression { add(60);}
     ;

%%

void yyerror (const char *msg)
{
	fprintf(stderr,"Syntax Error!\n");
}

