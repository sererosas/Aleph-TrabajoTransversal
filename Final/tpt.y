%{
#include <stdio.h>
#include "aleph.h"

DataType dt = NULL;

extern int yylex();
int yyerrork;
void yyerror(char *);
%}

%union{
    struct ast* a;
    struct symbol *s;
    struct symlist *sl;
    char* c;
    int cmp;
}

%type <a> exp
%type <a> literal_conj
%type <a> literal_list
%type <a> list_elem
%type <sl> parametros
%type <a> stmts
%type <a> aleph
%type <a> control
%type <a> cond


%nonassoc <cmp> CMP
%right '='
%left PUSH
%left UNION_CONCAT INTER DIFF
%left AND OR NOT
%right POP
%nonassoc CARDINAL SIZEL
%left ELEM
%left ID
%left PRINT
%left RETURN

%token <c> ELEM
%token <s> ID
%token <s> FUNC
%token WHILE IF FOR_EACH UNION_CONCAT INTER DIFF CARDINAL SIZEL POP IN CMP 
%token ELSE EOL AND OR PUSH NOT PROC PRINT DEF LET RETURN V F

%start aleph

%%
aleph: /*vacio*/            { $$ = NULL; printf("\naleph> "); }
    | aleph error EOL       { yyerrork; printf("\naleph> "); }
    | aleph stmts EOL       { dt = eval($2); freeast($2); printf("\naleph> "); }
    | aleph DEF FUNC '('parametros')' '{'stmts'}' EOL    { newfn($3, $5, $8); printf("\naleph> "); }
    | aleph DEF FUNC '('')' '{' stmts '}' EOL            { newfn($3, NULL, $7); printf("\naleph> "); }
    | aleph LET '('parametros')' '=' '('list_elem')' EOL { newasgnmult($4, $8); printf("\naleph> ")} 
; 

stmts: stmts ';'         { $$ = newast(';', $1, NULL); }
     | stmts exp ';'     { $$ = newast(';', $1, $2); }
     | stmts control     { $$ = newast(';', $1, $2); }
     | control           { $$ = $1; }
     | exp               { $$ = $1; }
     
;

control: IF '(' cond ')' '{' stmts '}' { $$ = newif(IF, $3, $6, NULL); }
       | IF '(' cond ')' '{' stmts '}' ELSE '{' stmts '}' { $$ = newif(IF, $3, $6, $10); }
       | WHILE '(' cond ')' '{' stmts '}' { $$ = newast(WHILE, $3, $6); }
       | FOR_EACH ID IN exp '{' stmts '}' { $$ = newfor(FOR_EACH, newasgn($2, NULL), $4, $6); }
;

exp:  exp UNION_CONCAT exp  { $$ = newast(UNION_CONCAT, $1, $3); }
    | exp INTER exp         { $$ = newast(INTER, $1, $3); }
    | exp DIFF exp          { $$ = newast(DIFF, $1, $3); }
    | POP exp               { $$ = newast(POP, NULL, $2); } 
    | exp PUSH exp          { $$ = newast(PUSH, $1, $3); }
    | CARDINAL exp          { $$ = newast(CARDINAL, NULL, $2); } 
    | SIZEL exp             { $$ = newast(SIZEL, NULL, $2); }
    | literal_conj          { $$ = $1; }
    | literal_list          { $$ = $1; }
    | '(' exp ')'           { $$ = $2; }
    | ID '=' exp            { $$ = newasgn($1,$3); }
    | ELEM                  { $$ = newelem(STR, $1); }
    | ID                    { $$ = newref($1); }
    | PRINT exp             { $$ = newast(PRINT, $2, NULL); }
    | FUNC '('list_elem')'  { $$ = newfncall($1, $3); }
    | FUNC '('')'           { $$ = newfncall($1, NULL); }
    | RETURN exp            { $$ = newast(RETURN, $2, NULL); }
;

cond: exp IN exp        { $$ = newast(IN, $1, $3); }
    | exp CMP exp       { $$ = newast($2, $1, $3); }
    | '(' cond ')'      { $$ = $2; }
    | cond AND cond     { $$ = newast(AND, $1, $3); }
    | cond OR cond      { $$ = newast(OR, $1, $3); }
    | NOT cond          { $$ = newast(NOT, $2, NULL); }
    | V                 { $$ = newast(V, NULL, NULL); }
    | F                 { $$ = newast(F, NULL, NULL); }
;

literal_conj: '{' list_elem '}' { $$ = newast(SET, $2, NULL); }
            | '{''}'            { $$ = newast(SET, NULL, NULL); }
;

literal_list: '[' list_elem ']' { $$ = newast(LIST, $2, NULL); }
            | '['']'            { $$ = newast(LIST, NULL, NULL); }
;

list_elem: exp ',' list_elem { $$ = newast('L', $1, $3); }
         | exp               { $$ = newast('L', $1, NULL); }
;

parametros: ID ',' parametros  { $$ = newsymlist($1, $3); }
          | ID                 { $$ = newsymlist($1, NULL); }
;

%%
void yyerror(char *s) {
    fprintf(stderr, "error: %s\n", s);
}

int main() {
    return yyparse();
}