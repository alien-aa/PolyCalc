%option noyywrap
%option yylineno
%option noinput
%option nounput

%{
#include "polynomial.h"
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
extern int yylineno;
char last_char = '\0';
%}

%%

[ \t]+                 {}
\n                     {}

"print"                { return PRINT; }
"eval"                 { return EVAL; }

[$@][A-Za-z_][A-Za-z0-9_]* { last_char = *yytext; yylval.str = strdup(yytext); return POLY_VAR; }

[a-zA-Z_][a-zA-Z0-9_]* { last_char = *yytext; yylval.str = strdup(yytext); return VARIABLE; }

[0-9]+                  { last_char = *yytext; yylval.num = atoi(yytext); return NUMBER; }

","                     { last_char = *yytext; return COMMA; }
"+"                     { last_char = *yytext; return ADD; }
"-"                     { last_char = *yytext; return SUB; }
"*"                     { last_char = *yytext; return MUL; }
"/"                     { last_char = *yytext; return DIV; }
"^"                     { last_char = *yytext; return POW; }
"("                     { last_char = *yytext; return LPAREN; }
")"                     { last_char = *yytext; return RPAREN; }
";"                     { last_char = *yytext; return SEMI; }
"="                     { last_char = *yytext; return ASSIGN; }

"//".*                  {}

.                       { 
                            last_char = *yytext; 
                            fprintf(stderr, "[ERR] Line %d: Lexical error: Invalid character '%c'\n", yylineno, *yytext); 
                            exit(1); 
                        }
