/* 
 * scanner.l - Scanner utility for the DHBW compiler
 */

%{
	#include "symboltable.h"
	#include "parser.h"
	int colnum = 0;
	#define YY_USER_ACTION {yylloc.first_line = yylineno; yylloc.first_column = colnum; colnum=colnum+yyleng; yylloc.last_column=colnum; yylloc.last_line = yylineno;}
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

%x COMMENT

/*
\!						return NOT;

*/

%%

\/\*					{BEGIN(COMMENT);}
<COMMENT>{ 
\*\/					%{BEGIN(INITIAL); %}
\n						%{ /* yylineno++;*/ %}
.						%{ /* multi line comment. Do nothing. */ %}
}

\/\/.*\n				{ /* one line comment. Do nothing. */ }

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
"<<"					return(SHIFT_LEFT);
">>"					return(SHIFT_RIGHT);

[A-Za-z_][A-Za-z_0-9]*	{ 	yylval.str = strdup(yytext);
							return ID;}
0|[1-9][0-9]*			{	yylval.num=atoi(yytext); 
							return NUM;}
\n						{ /* printf("\n");  nothing to do */}
[ ]						{/* nothing to do */}
<<EOF>>					return 0; // we don't need to handle this
							
%%

/*
int main() {
	while(1) {
		yylex();
	}
}
*/