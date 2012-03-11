/* 
 * scanner.l - Scanner utility for the DHBW compiler
 */

%{
	#include "parser.h"
%}

/* 
 * Default setting for flex which indicates that yytext, which is one of the main connections
 * between the scanner and parser, is handled as pointer of char instead of an array of char.
*/ 
%pointer

/* save the line number of terminals */
%option yylineno

/* finish after scanning the provided file */
%option noyywrap

/*
\!						return NOT;

*/

/*EOF if (yywrap( ))		return END_OF_FILE;*/
/*<<EOF>>					return END_OF_FILE;*/

%%

\/\/.*\n				;

do						return DO;
while					return WHILE;
if						return IF;
else					return ELSE;
int						return INT;
void					return VOID;
return					return RETURN;
:						return COLON;
,						return COMMA;
;						return SEMICOLON;
=						return ASSIGN;

\|\|					return LOGICAL_OR; 
!						return LOGICAL_NOT; 
&&						return LOGICAL_AND;

==						return EQ; 
!=						return NE; 
\<						return LS; 
\<=						return LSEQ; 
>=						return GTEQ; 
>						return GT;

\+						return PLUS; 
\-						return MINUS; 
\*						return MUL; 
\/						return DIV; 
\%						return MOD;

\[						return BRACKET_OPEN;
\]						return BRACKET_CLOSE;
\(						return PARA_OPEN; 
\)						return PARA_CLOSE;
\{						return BRACE_OPEN;
\}						return BRACE_CLOSE;

[A-Za-z_][A-Za-z_0-9]*	{ 	yylval.str = strdup(yytext); 
							return ID;}
0|[1-9][0-9]*			{	yylval.num=atoi(yytext); 
							return NUM;}
\n						{ printf("\n"); /* nothing to do */}
[ ]						{/* nothing to do */}
							
%%

/*
int main() {
	while(1) {
		yylex();
	}
}
*/