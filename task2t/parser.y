
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"

void yyerror(char *s, ...);
int yylex();
struct ast *root;
%}

%debug

%locations

%union {
    int intval;
    double floatval;
    char *strval;
    struct ast *astinfo;
}

	/* token 关键字 */
%token WHILE
%token IF
%token THEN
%token ELSE
%token DO
%token <strval> IDN
%token <intval> INT8
%token <intval> INT10
%token <intval> INT16

%left '<' '>'
%left '+' '-'
%left '*' '/'

%type <astinfo> P L S SP C CP E T F

%start P

%%

/* 语法规则 */
P: L            { $$ = newast(1, NULL, $1, NULL); root = $$;}
 | L P          { $$ = newast(2, $1, NULL, $2); root = $$;}
 ;

L: S ';'        { $$ = newast(3, NULL, $1, NULL); }
;

S: IDN '=' E            { $$ = newastwithidn(4, $1, NULL, $3, NULL); }
 | IF C THEN SP       { $$ = newast(5, $2, NULL, $4); }
 | WHILE C DO S         { $$ = newast(6, $2, NULL, $4); }
 | '{' P '}'            { $$ = newast(7, NULL, $2, NULL); }
 ;

SP: S           { $$ = newast(8, NULL, $1, NULL); }
  | S ELSE S    { $$ = newast(9, $1, NULL, $3); }

C: E CP         { $$ = newast(10, $1, NULL, $2); }
;

CP: '>' E       { $$ = newast(11, NULL, $2, NULL); }
  | '<' E       { $$ = newast(12, NULL, $2, NULL); }
  | '=' E       { $$ = newast(13, NULL, $2, NULL); }
  ;

E: T            { $$ = newast(14, NULL, $1, NULL); }
  | E '+' T     { $$ = newast(15, $1, NULL, $3); }
  | E '-' T     { $$ = newast(16, $1, NULL, $3); }
  ;

T: F            { $$ = newast(17, NULL, $1, NULL); }
  | T '*' F     { $$ = newast(18, $1, NULL, $3); }
  | T '/' F     { $$ = newast(19, $1, NULL, $3); }
  ;

F: '(' E ')'    { $$ = newast(20, NULL, $2, NULL); }
  | IDN         { $$ = newidn(21, $1); }
  | INT8        { $$ = newnum(22, $1); }
  | INT10       { $$ = newnum(23, $1); }
  | INT16       { $$ = newnum(24, $1); }
  ;


%%

int main(int argc, const char *args[])
{
	/* 将注释去掉就能看到stack具体是怎么工作的.. */
    yydebug = 1;

	extern FILE *yyin;

	if(argc > 1 && (yyin = fopen(args[1], "r")) == NULL) {
		fprintf(stderr, "can not open %s\n", args[1]);
		exit(1);
	}

	if(yyparse()) {
		exit(-1);
	}

    FILE * f;
    f = fopen("out.txt", "w+");
	displayAST(root, f);
    return 0;
}

void yyerror(char *s, ...)
{
    extern int yylineno;

    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}