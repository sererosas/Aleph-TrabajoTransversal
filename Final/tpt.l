%{
#include <stdio.h>
#include <string.h>
#include "tpt.tab.h"
#include "aleph.h"
%}
%%
","     |
";"     |  
"{"     |
"}"     |
"["     |
"]"     |
"("     |
")"     |
"="     { return yytext[0]; }

"&&" { return AND; }
"||" { return OR; }
"!" { return NOT; }
"true" { return V; }
"false" { return F; }

"==" { yylval.cmp = 4; return CMP; }
"!=" { yylval.cmp = 5; return CMP; }
"<"  { yylval.cmp = 6; return CMP; }
">"  { yylval.cmp = 7; return CMP; }
"<=" { yylval.cmp = 8; return CMP; }
">=" { yylval.cmp = 9; return CMP; }

"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
"for" { return FOR_EACH; }

"+" { return UNION_CONCAT; }
"^" { return INTER; }
"-" { return DIFF; }
"cardinal" { return CARDINAL; }
"sizel"  {return SIZEL; }
"pop" { return POP; }
"push" { return PUSH; }
"in" { return IN; }

"let" { return LET; }
"define" { return DEF; }
"return" { return RETURN; }

"print" { return PRINT; }

"'"[a-zA-Z0-9]*"'" { yylval.s = lookup(yytext); return ID; }
[a-zA-Z0-9]+ { yylval.c = strdup(yytext); return ELEM; }
"fn_"[a-zA-Z0-9]+ { yylval.s = lookup(yytext); return FUNC; }


"\n" { return EOL; }
[ \t] {}
%%
