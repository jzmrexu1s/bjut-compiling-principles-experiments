/* Created by Chengwei Zhang on 5/21/20. */
//todo
//gen newlabel
//place true false next newtemp value
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"
extern char *yytext;
void yyerror(char *s);
int yylex();

void pplace(struct astNode *node);
void gen(struct astNode* result,char op,struct astNode* arg1,struct astNode* arg2);
struct astNode* newtemp(struct astNode* node);

struct astNode *root;
int cnt = 0;
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

S: IDN '=' E            { $$ = createAstNodeIdn(4, $1, NULL, $3, NULL); gen($$,'=',$3,NULL);}
 | IF C THEN SP       { $$ = createAstNode(5, $2, NULL, $4); }
 | WHILE C DO S         { $$ = createAstNode(6, $2, NULL, $4); }
 | '{' P '}'            { $$ = createAstNode(7, NULL, $2, NULL); }
 ;

SP: S           { $$ = createAstNode(8, NULL, $1, NULL); }
  | S ELSE S    { $$ = createAstNode(9, $1, NULL, $3); }
  ;

C: E CP         { $$ = createAstNode(10, $1, NULL, $2); }
 ;

CP: '>' E       { $$ = createAstNode(11, NULL, $2, NULL); }
  | '<' E       { $$ = createAstNode(12, NULL, $2, NULL); }
  | '=' E       { $$ = createAstNode(13, NULL, $2, NULL); }
  ;

E: T            { $$ = createAstNode(14, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | E '+' T     { $$ = createAstNode(15, $1, NULL, $3);  $$ = newtemp($$); gen($$,'+',$1,$3);}
  | E '-' T     { $$ = createAstNode(16, $1, NULL, $3);  $$ = newtemp($$); gen($$,'-',$1,$3);}
  ;

T: F            { $$ = createAstNode(17, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | T '*' F     { $$ = createAstNode(18, $1, NULL, $3);  $$ = newtemp($$); gen($$,'*',$1,$3);}
  | T '/' F     { $$ = createAstNode(19, $1, NULL, $3);  $$ = newtemp($$); gen($$,'/',$1,$3);}
  ;

F: '(' E ')'    { $$ = createAstNode(20, NULL, $2, NULL); }
  | IDN         { $$ = createIdn(21, $1); }
  | INT8        { $$ = createNum(22, $1); }
  | INT10       { $$ = createNum(23, $1); }
  | INT16       { $$ = createNum(24, $1); }
  ;


%%
//$$->idn=newtemp();
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

struct astNode* newtemp(struct astNode* node){
  cnt++;
  node->t=cnt;
  node->type=5;
  return node;
}

void gen(struct astNode* result,char op,struct astNode* arg1,struct astNode* arg2){
  pplace(result);
  printf(" := ");
  if(op=='='){
    pplace(arg1);
  }else{
    pplace(arg1);
    printf(" %c ",op);
    pplace(arg2);
  }
  printf("\n");
}

void pplace(struct astNode *node){
  switch(node->type){
    case 0:
      printf("%s",node->idn);
      break;
    case 1:
      printf("%d",node->num);
      break;
    case 5:
      printf("t%d",node->t);
      break;
  }
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
