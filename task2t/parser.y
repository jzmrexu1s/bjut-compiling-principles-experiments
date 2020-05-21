
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
    char *strval;
    struct astNode *node;
}

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

%type <node> P L S SP C CP E T F

%start P

%%

P: L            { $$ = createAstNode(1, NULL, $1, NULL); root = $$;}
 | L P          { $$ = createAstNode(2, $1, NULL, $2); root = $$;}
 ;

L: S ';'        { $$ = createAstNode(3, NULL, $1, NULL); }
;

S: IDN '=' E            { $$ = createAstNodeIdn(4, $1, NULL, $3, NULL); }
 | IF C THEN SP       { $$ = createAstNode(5, $2, NULL, $4); }
 | WHILE C DO S         { $$ = createAstNode(6, $2, NULL, $4); }
 | '{' P '}'            { $$ = createAstNode(7, NULL, $2, NULL); }
 ;

SP: S           { $$ = createAstNode(8, NULL, $1, NULL); }
  | S ELSE S    { $$ = createAstNode(9, $1, NULL, $3); }

C: E CP         { $$ = createAstNode(10, $1, NULL, $2); }
;

CP: '>' E       { $$ = createAstNode(11, NULL, $2, NULL); }
  | '<' E       { $$ = createAstNode(12, NULL, $2, NULL); }
  | '=' E       { $$ = createAstNode(13, NULL, $2, NULL); }
  ;

E: T            { $$ = createAstNode(14, NULL, $1, NULL); }
  | E '+' T     { $$ = createAstNode(15, $1, NULL, $3); }
  | E '-' T     { $$ = createAstNode(16, $1, NULL, $3); }
  ;

T: F            { $$ = createAstNode(17, NULL, $1, NULL); }
  | T '*' F     { $$ = createAstNode(18, $1, NULL, $3); }
  | T '/' F     { $$ = createAstNode(19, $1, NULL, $3); }
  ;

F: '(' E ')'    { $$ = createAstNode(20, NULL, $2, NULL); }
  | IDN         { $$ = createIdn(21, $1); }
  | INT8        { $$ = createNum(22, $1); }
  | INT10       { $$ = createNum(23, $1); }
  | INT16       { $$ = createNum(24, $1); }
  ;


%%

int main(int argc, const char *args[])
{
	/* 去除注释开启debug模式 */
    /* yydebug = 1; */

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
	printTree(root, f);
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