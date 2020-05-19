%option noyywrap
%{
#include <stdio.h>
#include <math.h>
#include<stdlib.h>
#include <string.h>

#define FUHAO 1
#define IDN 2
#define INT8 3
#define INT10 4
#define INT16 5
#define FLOAT8 6
#define FLOAT10 7
#define FLOAT16 8

#define IF 11
#define THEN 12
#define ELSE 13
#define WHILE 14
#define DO 15

int yylval;
%}

digit	[0-9]
letter	[A-Za-z]
hex		[0-9a-f]
idn		{letter}({letter}|{digit})*([_.]({letter}|{digit})+)?
int8	0[0-7]+
int10	0|([1-9]{digit}*)
int16	0x{hex}+
float8	0[0-7]+[.][0-7]+
float10	(0|[1-9]{digit}*)[.]{digit}+
float16	0x{hex}+[.]{hex}+
fuhao	[+]|[-]|[*]|[/]|[>]|[<]|[=]|[(]|[)]|[;]

%%

while		{return WHILE;}
if			{return IF;}
then		{return ELSE;}
else		{return WHILE;}
do			{return DO;}
{fuhao}		{yylval=FUHAO;return yylval;}
{idn}		{yylval=IDN;return yylval;}
{int10}		{yylval=INT10;return yylval;}
{int8}		{yylval=INT8;return yylval;}
{int16}		{yylval=INT16;return yylval;}
{float10}	{yylval=FLOAT10;return yylval;}
{float8}	{yylval=FLOAT8;return yylval;}
{float16}	{yylval=FLOAT16;return yylval;}

.			{;}


%%
int htoi(char s[]);
int otoi(char *p);
char* itoa(int num,char *str);
char* ftoa(float num,char *str);
char* fotoa(char *p);
char* fhtoa(char *p);
void myprint(int c){
	switch(c){
		case FUHAO:printf("%s\t\t_\n",yytext);break;
		case IDN:printf("IDN\t\t%s\n",yytext);break;
		case INT8:printf("INT8\t\t%d\n",otoi(yytext));break;
		case INT10:printf("INT10\t\t%d\n",atoi(yytext));break;
		case INT16:printf("INT16\t\t%d\n",htoi(yytext));break;
		case FLOAT8:printf("FLOAT8\t\t%s\n",fotoa(yytext));break;
		case FLOAT10:printf("FLOAT10\t\t%g\n",atof(yytext));break;
		case FLOAT16:printf("FLOAT16\t\t%s\n",fhtoa(yytext));break;
		case WHILE:printf("WHILE\t\t_\n");break;
		case IF:printf("IF\t\t_\n");break;
		case THEN:printf("THEN\t\t_\n");break;
		case ELSE:printf("ELSE\t\t_\n");break;
		case DO:printf("DO\t\t_\n");break;
	}
}

int main(void){
	yyin=stdin;
	int c;
	while (c = yylex()){
		myprint(c);
	}
	return 0;
}

char* itoa(int num,char *str)
{
    int i = 0;
    if(num<0) {
        num = -num;
        str[i++] = '-';
    }
    do {
        str[i++] = num%10+48;
        num /= 10;
    }while(num);
    str[i] = '\0';
    int j = 0;
    if(str[0]=='-') {
        j = 1;
        ++i;
    }
    for(;j<i/2;j++) {
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    }
    return str;
}

char* ftoa(float num,char *str)
{
    int i = 0;
    if(num<0) {
        num = -num;
        str[i++] = '-';
    }
    do {
		num *= 10;
		int tmp=floor(num);
		num -= tmp;
        str[i++] = tmp%10+48;
    }while(num>0);
    str[i] = '\0';
    int j = 0;
    if(str[0]=='-') {
        j = 1;
        ++i;
    }
    /*for(;j<i/2;j++) {
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    }*/
    return str;
}


char* fotoa(char *p) {
    int fcount = 0;
    int i = 0;
    int f = 0;
    while (*p != '.') {
        i = i * 8 + *p -'0';
        p ++;
    }
    p ++;
    while (*p != '\0') {
        f = f * 8 + *p - '0';
        p ++;
        fcount ++;
    }
    for (int j = 0; j < fcount; j ++) {
        f *= 1000 / 8;
    }
    char s[100];
    sprintf(s, "%d.%0*d", i, fcount*3, f);
    char* ps = s;
    return ps;
}

char* fhtoa(char *p){
    int fcount = 0;
    int i = 0;
    float f = 0;
	while (*p!='x') p ++;
	p ++;
    while (*p != '.') {
		int tmp;
		if ( *p <= '9') tmp = *p - '0';
		else tmp = 10 + *p - 'a';
        i = i * 16 + tmp;
        p ++;
    }
    p ++;
    while (*p != '\0') {
		int tmp;
		if ( *p <= '9') tmp = *p - '0';
		else tmp = 10 + *p - 'a';
        f = f * 16 + tmp;
        p ++;
        fcount ++;
    }
    for (int j = 0; j < fcount; j ++) {
        f *= 0.0625;
    }
    char si[100];
    char sf[100];
    char s[100];
    itoa(i, si);
	ftoa(f, sf);
    strcat(si, ".");
    strcpy(s, si);
    strcat(s, sf);
    char* ps = s;
    return ps;
}

int otoi(char *p)
{   
	int n;
    n= *p-'0';
    p++;
    while(*p!='\0') {
    	n=n*8+*p-'0';
    	p++;
    }
    return n;
}

int htoi(char s[])
{
    int i=2;
    int n=0;
    for(;s[i]!='\0';++i)
    {
        if(s[i]>'9')
        {
            n=16*n+(10+s[i]-'a');
        }
        else
        {
            n=16*n+(s[i]-'0');
        }
    }
    return n;
}
