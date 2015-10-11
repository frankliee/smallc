%{
#include <stdio.h>
#include "lib.h"
#include "small.tab.h"
#include <iostream>
using namespace std;
%}
%option nodefault
%option noyywrap 
%option yylineno
%x COMMENT

%%

"//".*\n | 
"%".*\n  { }

"/*" { BEGIN(COMMENT); }
<COMMENT>([^*]|\n)+|. { }
<COMMENT>"*/" { BEGIN(INITIAL);}
<COMMENT><<EOF>> { 	printf("%d: Unterminated comment\n",yylineno); }


[0-9]+(.[0-9]+)? { 	yylval.d = atof(yytext); return NUMBER; }

"++"	{	return DADD;}
"--"	{	return DSUB;}

"+" {	return '+';}
"-" {	return '-';}
"*" {	return '*';}
"/" {	return '/';}
"," {	return ',';}
";" {	return ';';}
"(" {	return '(';}
")" {	return ')';}
"[" {	return '[';}
"]" {	return ']';}
"{" {	return '{';}
"}" {	return '}';}
"mod" {	return '%';}
"|" {	return '|';}
"^" {	return '^';}
"&" {	return '&';}
"~"	{	return '~';}
"=" {	return '=';}

'[^']*'	{	yylval.str = strdup(yytext); return STRING; }

">"  { yylval.type=1; return CMP;}
"<"  { yylval.type=2; return CMP;}
"!=" { yylval.type=3; return CMP;}
"==" { yylval.type=4; return CMP;}
">=" { yylval.type=5; return CMP;}
"<=" { yylval.type=6; return CMP;}

"and"	{	return AND;}
"or"  	{ 	return OR;}
"not"	{	return NOT;}
"if" 	{	return IF;}
"else"	{	return ELSE;}
"while" {	return WHILE;}
"do"	{	return DO;	}
"for"	{	return FOR;	}
"repeat" {	return REPEAT;}
"until"	{	return UNTIL;}
"read"	{	return READ;}
"print" {	return WRITE;}
"write"	{	return WRITE;}
"int"	{ 	return VAR;}
"var"   {	return VAR;}
"exit"	{	return EXIT;}
"abs"	{	return ABS;}
"sqrt"	{	return SQRT;}
"pow"	{	return POW;}
"odd"	{	return ODD;}
"true"	{	yylval.d = 1;return NUMBER;}
"false" {	yylval.d = 0;return NUMBER;}
"debug"	{	return DEBUG;}

[ \t]   { /* ignore white space */ }
\n		{ 				}


[a-zA-Z][a-zA-Z0-9]* {
	 yylval.sym = lookup(yytext);
	 return ID;
}

.	{printf("Mystery character %c\n", *yytext); }

