/* Created by Chengwei Zhang on 5/21/20. */
//todo
//gen newlabel
//place true false next newtemp value
%{
//GEN means the valid postion that you can add to in the last of string
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

/*
gen() is a method which can generate the 3AC code to gen_str[nextquad], then nextquad will ++.
there are some different usage in this method and you can look for more details in the code.
*/
void gen(struct astNode* result,char op,struct astNode* arg1,struct astNode* arg2,int extra);
//create a new temporary variable for "node", and let the type to 5
struct astNode* newtemp(struct astNode* node);
struct astNode *root;

// use "quad" backpatch all lines in "p"
void backpatch(struct listNode* p,int quad);
//the total number of temporary variable
int cnt;
//means the next line number of the 3AC code forever
int nextquad = 0;
//the starting line number
int startquad = 100;
//the space to save the 3AC code
char *gen_str[100];
%}

%debug
%locations
%union {
    int intval;
    double doubleval;
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
%token <doubleval> FLOAT8
%token <doubleval> FLOAT10
%token <doubleval> FLOAT16

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
 | IDN error '=' E      { yyerrok; }
 | IDN '=' error E      { yyerrok; }
 | IDN error            { yyerrok; }
 | error '=' error      { yyerrok; }
 | IF C THEN M SP       { $$ = createAstNode(5, $2, NULL, $5); 
                          backpatch($2->truelist,$4); 
                          $$->nextlist=merge($2->falselist,$5->nextlist); 
                          if($5->type==6)  backpatch($2->falselist,$5->quad);}
 | IF error C THEN SP    { yyerrok; }
 | IF C error THEN SP    { yyerrok; }
 | IF C THEN error SP    { yyerrok; }
 | IF error THEN SP      { yyerrok; }
 | WHILE M C DO M S         { $$ = createAstNode(6, $3, NULL, $6); backpatch($6->nextlist,$2); backpatch($3->truelist,$5); $$->nextlist=merge($$->nextlist,$3->falselist); gen(NULL,'9',NULL,NULL,$2);}
 | WHILE error M C DO M S       { yyerrok; }
 | WHILE M C error DO M S       { yyerrok; }
 | WHILE M C error DO error M S     { yyerrok; }
 | '{' P '}'            { $$ = createAstNode(7, NULL, $2, NULL); }
 | IF C M SP	{ printf("expected 'then' before '%s' ",yytext); yyerror("missing THEN"); }
 | IF C F	{ printf("expected 'then' before '%s' ",yytext);yyerror("missing THEN"); }
 | WHILE M C M S	{ printf("expected 'do' before '%s' \n",yytext); yyerror("missing DO");}
 | WHILE M C E	{ printf("expected 'do' before '%s' \n",yytext); yyerror("missing DO");}
 | DO	{ printf("WHILE statement not detected before 'do' \n");yyerror("missing WHILE");}
 
 ;

SP: S           { $$ = createAstNode(8, NULL, $1, NULL); $$->nextlist=$1->nextlist;$$->truelist=$1->truelist;$$->falselist=$1->falselist;}
  | S M ELSE M S    { $$ = createAstNode(9, $1, NULL, $5); gen(NULL,'0',NULL,NULL,9); $$->nextlist=merge(makelist($2),$5->nextlist); $$->quad=$4; $$->type=6; }
  ;

C: E CP         { $$ = createAstNode(10, $1, NULL, $2); 
                  $$ -> truelist = makelist(nextquad); 
                  $$ -> falselist = makelist(nextquad+1);
                  gen(NULL,$2->relop,$1,$2,1); 
                  gen(NULL,'0',NULL,NULL,9); }

 | E error CP   { yyerrok; }
 | error CP     { yyerrok; }
  
 ;

CP: '>' E       { $$ = createAstNode(11, NULL, $2, NULL); $$ -> relop = '>';}
  | '<' E       { $$ = createAstNode(12, NULL, $2, NULL); $$ -> relop = '<';}
  | '=' E       { $$ = createAstNode(13, NULL, $2, NULL); $$ -> relop = '=';}
  | '>' error E { yyerrok; }
  | '<' error E { yyerrok; }
  | '=' error E { yyerrok; }
  ;

E: T            { $$ = createAstNode(14, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | E '+' T     { $$ = createAstNode(15, $1, NULL, $3);  $$ = newtemp($$); gen($$,'+',$1,$3,0);}
  | E '-' T     { $$ = createAstNode(16, $1, NULL, $3);  $$ = newtemp($$); gen($$,'-',$1,$3,0);}
  | E '+' error T { yyerrok; }
  | E '-' error T { yyerrok; }
  ;

T: F            { $$ = createAstNode(17, NULL, $1, NULL); $$ -> type = $1 ->type;}
  | T '*' F     { $$ = createAstNode(18, $1, NULL, $3);  $$ = newtemp($$); gen($$,'*',$1,$3,0);}
  | T '/' F     { $$ = createAstNode(19, $1, NULL, $3);  $$ = newtemp($$); gen($$,'/',$1,$3,0);}
  | T '*' error F { yyerrok; }
  | T '/' error F { yyerrok; }
  ;

F: '(' E ')'    { $$ = createAstNode(20, NULL, $2, NULL); }
  | IDN         { $$ = createIdn(21, $1); }
  | INT8        { $$ = createNum(22, $1); }
  | INT10       { $$ = createNum(23, $1); }
  | INT16       { $$ = createNum(24, $1); }
  | FLOAT8      { $$ = createNum(25, $1); }
  | FLOAT10     { $$ = createNum(26, $1); }
  | FLOAT16     { $$ = createNum(27, $1); }
  | '('E error  { yyerrok; }
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
  printGencode(f);
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
  sprintf(GEN,"%d:\t",nextquad+startquad);
  if(op=='9'){
    sprintf(GEN,"goto %d",extra+startquad);
  }
  else if(extra==0){
    pplace(result);
    sprintf(GEN," := ");
    if(op=='='){
      pplace(arg1);
    }else{{ $$ = createAstNode(9, $1, NULL, $5); gen(NULL,'0',NULL,NULL,9); $$->next
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
    sprintf(GEN,"goto ");
  }
  nextquad++;
}

void backpatch(struct listNode* p,int quad){
    while(p){
        sprintf(gen_str[p->quad]+strlen(gen_str[p->quad]),"%d",quad+startquad);
        p=p->next;
    }
}

//generate the variable name to 3AC
void pplace(struct astNode *node){
  switch(node->type){
    case 0:
      sprintf(GEN,"%s",node->idn);
      break;
    case 1:
      sprintf(GEN,"%g",node->num);
      break;
    case 5:
      sprintf(GEN,"t%d",node->t);
      break;
  }
}

void printGencode(FILE* f){
  fprintf(f,"\n");
  for(int i=0;i<nextquad;i++){
    fprintf(f,"%s\n",gen_str[i]);
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
