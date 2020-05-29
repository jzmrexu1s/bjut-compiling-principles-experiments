/* Created by Chengwei Zhang on 5/21/20. */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"
extern char *yytext;
void yyerror(char *s);
int yylex();
struct astNode *root;
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
 | S error ';'  { yyerrok; }
;

S: IDN '=' E            { $$ = createAstNodeIdn(4, $1, NULL, $3, NULL); }
 | IF C THEN SP       { $$ = createAstNode(5, $2, NULL, $4); }
 | IF error C THEN SP    { yyerrok; }
 | IF C error THEN SP    { yyerrok; }
 | IF C THEN error SP    { yyerrok; }
 | IF error THEN SP      { yyerrok; }
 | WHILE C DO S         { $$ = createAstNode(6, $2, NULL, $4); }
 | WHILE error C DO S	{ yyerrok;}
 | WHILE C error DO S	{ yyerrok;}
 | WHILE C DO error S	{ yyerrok;}
 | '{' P '}'            { $$ = createAstNode(7, NULL, $2, NULL); }
 | IDN error '=' E      { yyerrok; }
 | IDN '=' error E      { yyerrok; }
 | IDN error            { yyerrok; }
 | error '=' error      { yyerrok; }
 | IF C SP	{ printf("expected 'then' before '%s' ",yytext); yyerrok; }
 | IF C F	{ printf("expected 'then' before '%s' ",yytext); yyerrok; }
 | WHILE C S	{ printf("expected 'do' before '%s' \n",yytext); yyerror("missing DO");}
 | WHILE C E	{ printf("expected 'do' before '%s' \n",yytext); yyerror("missing DO");}
 | WHILE C DO E	{ printf("warning 'do %s' seems meaningless \n\n",yytext-1);}
 | DO	{ printf("expected WHILE before do \n");yyerror("missing WHILE");}
 ;

SP: S           { $$ = createAstNode(8, NULL, $1, NULL); }
  | S ELSE S    { $$ = createAstNode(9, $1, NULL, $3); }
  | S ELSE error S      { yyerrok; }
  ;

C: E CP         { $$ = createAstNode(10, $1, NULL, $2); }
 | E error CP   { yyerrok; }
 ;

CP: '>' E       { $$ = createAstNode(11, NULL, $2, NULL); }
  | '<' E       { $$ = createAstNode(12, NULL, $2, NULL); }
  | '=' E       { $$ = createAstNode(13, NULL, $2, NULL); }
  ;

E: T            { $$ = createAstNode(14, NULL, $1, NULL); }
  | E '+' T     { $$ = createAstNode(15, $1, NULL, $3); }
  | E '-' T     { $$ = createAstNode(16, $1, NULL, $3); }
  | E '+' error T { yyerrok; }
  | E '-' error T { yyerrok; }
  ;

T: F            { $$ = createAstNode(17, NULL, $1, NULL); }
  | T '*' F     { $$ = createAstNode(18, $1, NULL, $3); }
  | T '/' F     { $$ = createAstNode(19, $1, NULL, $3); }
  | T '*' error F { yyerrok; }
  | T '/' error F { yyerrok; }
  ;

F: '(' E ')'    { $$ = createAstNode(20, NULL, $2, NULL); }
  | IDN         { $$ = createIdn(21, $1); }
  | INT8        { $$ = createNum(22, $1); }
  | INT10       { $$ = createNum(23, $1); }
  | INT16       { $$ = createNum(24, $1); }
  | '('E error  { yyerrok; }
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

void yyerror(char *s)
{
    extern int yylineno;
    extern YYLTYPE yylloc;
    //extern char yytext [];
    int errflag = 1;
    int start = yylloc.first_column;
    int end = yylloc.last_column;
    int i;
    printf("Unexpected '%s' \n",yytext);
    fprintf(stderr, "Error: %s on Line: %d:c%d to %d:c%d\n\n", s, yylineno, start, yylineno, end);

}
