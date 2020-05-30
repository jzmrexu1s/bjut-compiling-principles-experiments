/* Created by Chengwei Zhang on 5/21/20. */
//todo
//gen newlabel
//place true false next newtemp value
%{
#define GEN gen_str[nextquad]+strlen(gen_str[nextquad])
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "parser.h"

extern char *yytext;
void yyerror(char *s);
int yylex();
void printGencode();
void pplace(struct astNode *node);
void gen(struct astNode* result,char op,struct astNode* arg1,struct astNode* arg2,int extra);
struct astNode* newtemp(struct astNode* node);
struct astNode *root;
void backpatch(struct listNode* p,int quad);
int cnt = 0;
int nextquad = 0;
char *gen_str[100];
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
%type <intval> M

%start P

%%


P: L            { $$ = createAstNode(1, NULL, $1, NULL); root = $$;}
 | L P          { $$ = createAstNode(2, $1, NULL, $2); root = $$;}
 ;

L: S ';' M       { $$ = createAstNode(3, NULL, $1, NULL); backpatch($1->nextlist,$3);}
;

S: IDN '=' E            { $$ = createAstNodeIdn(4, $1, NULL, $3, NULL); gen($$,'=',$3,NULL,0);}
 | IF C THEN M SP       { $$ = createAstNode(5, $2, NULL, $5); backpatch($2->truelist,$4); $$->nextlist=merge($2->falselist,$5->nextlist); if($5->type==6)  backpatch($2->falselist,$5->quad);}
 | WHILE M C DO M S         { $$ = createAstNode(6, $3, NULL, $6); backpatch($6->nextlist,$2); backpatch($3->truelist,$5); $$->nextlist=merge($$->nextlist,$3->falselist); gen(NULL,'9',NULL,NULL,$2);}
 | '{' P '}'            { $$ = createAstNode(7, NULL, $2, NULL); }
 ;

SP: S           { $$ = createAstNode(8, NULL, $1, NULL); $$->nextlist=$1->nextlist;$$->truelist=$1->truelist;$$->falselist=$1->falselist;}
  | S M ELSE M S    { $$ = createAstNode(9, $1, NULL, $5); gen(NULL,'0',NULL,NULL,9); $$->nextlist=merge(makelist($2),$5->nextlist); $$->quad=$4; $$->type=6; }
  ;

C: E CP         { $$ = createAstNode(10, $1, NULL, $2); $$ -> truelist = makelist(nextquad); $$ -> falselist = makelist(nextquad+1);
                  gen(NULL,$2->relop,$1,$2,1); gen(NULL,'0',NULL,NULL,9); }
 ;

CP: '>' E       { $$ = createAstNode(11, NULL, $2, NULL); $$ -> relop = '>';}
  | '<' E       { $$ = createAstNode(12, NULL, $2, NULL); $$ -> relop = '<';}
  | '=' E       { $$ = createAstNode(13, NULL, $2, NULL); $$ -> relop = '=';}
  ;

E: T            { $$ = createAstNode(14, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | E '+' T     { $$ = createAstNode(15, $1, NULL, $3);  $$ = newtemp($$); gen($$,'+',$1,$3,0);}
  | E '-' T     { $$ = createAstNode(16, $1, NULL, $3);  $$ = newtemp($$); gen($$,'-',$1,$3,0);}
  ;

T: F            { $$ = createAstNode(17, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | T '*' F     { $$ = createAstNode(18, $1, NULL, $3);  $$ = newtemp($$); gen($$,'*',$1,$3,0);}
  | T '/' F     { $$ = createAstNode(19, $1, NULL, $3);  $$ = newtemp($$); gen($$,'/',$1,$3,0);}
  ;

F: '(' E ')'    { $$ = createAstNode(20, NULL, $2, NULL); }
  | IDN         { $$ = createIdn(21, $1); }
  | INT8        { $$ = createNum(22, $1); }
  | INT10       { $$ = createNum(23, $1); }
  | INT16       { $$ = createNum(24, $1); }
  ;

M: %empty       { $$ = nextquad; }
  ;

%%
int main(int argc, const char *args[])
{
	/* 去除注释开启debug模式 */
  //   yydebug = 1; 

	extern FILE *yyin;
  if(argc == 1 && (yyin = fopen("in.txt", "r")) == NULL){
    fprintf(stderr, "can not open in.txt\n");
		exit(1);
  }
	else if(argc > 1 && (yyin = fopen(args[1], "r")) == NULL) {
		fprintf(stderr, "can not open %s\n", args[1]);
		exit(1);
	}
	if(yyparse()) {
		exit(-1);
	}
    FILE * f;
    f = fopen("out.txt", "w+");
	printTree(root, f);
  printGencode();
    return 0;
}

struct astNode* newtemp(struct astNode* node){
  cnt++;
  node->t=cnt;
  node->type=5;
  return node;
}

void gen(struct astNode* result,char op,struct astNode* arg1,struct astNode* arg2,int extra){
  gen_str[nextquad]=(char *)malloc(sizeof(char)*100);
  sprintf(GEN,"%d:\t",nextquad);
  if(op=='9'){
    sprintf(GEN,"goto %d",extra);
  }
  else if(extra==0){
    pplace(result);
    sprintf(GEN," := ");
    if(op=='='){
      pplace(arg1);
    }else{
      pplace(arg1);
      sprintf(GEN,"%c",op);
      pplace(arg2);
    }
  }
  else if(extra==1){
    sprintf(GEN,"if ");
    pplace(arg1);
    sprintf(GEN," %c ",op);
    pplace(arg2);
    sprintf(GEN," goto ");
  }
  else if(extra==9){
    sprintf(GEN,"goto2 ");
  }
  //printf("%s\n",gen_str[nextquad]);
  nextquad++;
}

void backpatch(struct listNode* p,int quad){
    char tmp[4];
    itoa(quad,tmp,10);
    while(p){
        strcat(gen_str[p->quad],tmp);
        p=p->next;
    }
}

void pplace(struct astNode *node){
  switch(node->type){
    case 0:
      sprintf(GEN,"%s",node->idn);
      break;
    case 1:
      sprintf(GEN,"%d",node->num);
      break;
    case 5:
      sprintf(GEN,"t%d",node->t);
      break;
  }
}

void printGencode(){
  for(int i=0;i<nextquad;i++){
    printf("%s\n",gen_str[i]);
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