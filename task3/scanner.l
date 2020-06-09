%option noyywrap yylineno
%{
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
void yyerror(char *s, ...);
int yycolumn = 1;
int htoi(char s[]);
int otoi(char *p);
char* itoa(int num,char *str);
char* ftoa(float num,char *str);
double aotof(char *p);
double ahtof(char *p);
#define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno;            \
        yylloc.first_column = yycolumn; yylloc.last_column = yycolumn+yyleng-1; \
        yycolumn += yyleng;
#define FUHAO 1
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
fuhao	[+]|[-]|[*]|[/]|[>]|[<]|[=]|[(]|[)]|[;]|[{]|[}]

%%

while		{return WHILE;}
if			{return IF;}
then		{return THEN;}
else		{return ELSE;}
do			{return DO;}
{fuhao}		{return yytext[0];}
{idn}		{yylval.strval = strdup(yytext); return IDN;}
{int10}		{yylval.intval = atoi(yytext); return INT10;}
{int8}		{yylval.intval = otoi(yytext); return INT8;}
{int16}		{yylval.intval = htoi(yytext); return INT16;}
{float10}	{yylval.doubleval = atof(yytext); return FLOAT10;}
{float8}	{yylval.doubleval = aotof(yytext); return FLOAT8;}
{float16}	{yylval.doubleval = ahtof(yytext); return FLOAT16;}

.			{;}
\n          {yycolumn = 1;}


%%

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


double aotof(char *p) {
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
    return atof(s);
}

double ahtof(char *p){
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
    return atof(s);
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
