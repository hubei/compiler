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

%%
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
\(						return BRACE_OPEN;
\)						return BRACE_CLOSE;
EOF if (yywrap( ))		return END_OF_FILE
[A-Za-z_][A-Za-z_0-9]*	return ID;
0|[1-9][0-9]*			return NUM;
\/\/.*\n				return comment;
