%{
#include "small.tab.h"
#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <set>
#include <list>
#include <memory>
#include <iostream>
#define YYDEBUG 1 	
using namespace std;
int yylex();
int yyrestart(FILE * f);
void yyerror(char* s,...);
void lyyerror(YYLTYPE t,char * s,...);
int init(int argc,char** argv);
void end();
FILE * c_file;
int error_tag = 1;
%}
%debug
%locations
%union{
	struct Ast * ast;
	double d;
	char * str;
	struct Symbol* sym;
	struct SymList* symlist;
	int type;
}

%token DEBUG
%token <d> NUMBER
%token IF ELSE WHILE VAR CONST REPEAT UNTIL DO  FOR
%token READ WRITE EXIT ABS SQRT POW ODD
%token DADD DSUB AND OR NOT  
%token DEF RET
%token <str> STRING
%token <sym> ID
%token <type> BCALL
%token <type> CMP


%right '='
%left NOT
%left AND
%right OR
%nonassoc  CMP
%left '|' '&' '^' '%'
%left '+' '-'
%left '*' '/'
%nonassoc DADD DSUB
%nonassoc '~' UMINUS  /*  负号 */

%start Program 
%type <symlist> Sym_List
%type <ast> Decl_List State_List State 
%type <ast>  If_State While_State Exp Exp_List 

%%

Program : Decl_List State_List  {
	if(error_tag){
		eval($2);
//	table_show();
		treefree($2);
		tablefree();
	}
	else{
		cout<<"cannot execute for synax mistake"<<endl;
	}
//	cout<<endl<<"*******run end*******"<<endl;
	}
		;

Decl_List : {$$=NULL; }          
		 | Decl_List VAR Sym_List ';' {
				 $$=NULL;
		 	}
		 | Decl_List VAR Sym_List  { lyyerror(@1,"缺少 ';' ");  }
		 ; 
		
Sym_List:    { $$ = NULL; } 
		| ID { $$ = newSymList($1); } 
		| Sym_List ',' ID  { $$ = insertSymList('b',$3,$1); }
		| Sym_List ','     { lyyerror(@1,"多余 ','");}
		;

State_List: {$$=NULL;}
		| State_List State {
			if($1==NULL) $$ = $2;
			else $$ = newAst('L',$1,$2);
			} 	
		;	
Exp_List: {$$=NULL;}
		| Exp     { $$=$1;}
		| Exp_List ',' Exp { $$ = newAst('L',$1,$3); }
		| Exp_List ','     { lyyerror(@1,"多余 ',' "); }
		;
State:  Exp   ';'      { $$=$1; }
	 |  If_State     { $$=$1; }
	 |  While_State   { $$=$1; }
	 |  Exp				{ lyyerror(@1,"缺少 ;");}
	 ;

If_State: IF '(' Exp ')' '{' State_List '}'	ELSE '{'  State_List '}'
			{ $$=newFlow(IF,$3,$6,$10); }
		| IF '(' Exp ')' '{' State_List '}'
			{ $$=newFlow(IF,$3,$6,NULL);}
		| IF '(' Exp ')'  { lyyerror(@1, "缺少if主体"); }
		;

While_State: WHILE '(' Exp ')' '{' State_List '}'
					{ $$=newFlow(WHILE,$3,$6,NULL); }
		 | WHILE '(' Exp ')' 
					{ lyyerror(@1,"缺少while主体"); }
		 | DO '{' State_List '}' WHILE '(' Exp ')' ';'
					{ $$=newFlow(DO,$7,$3,NULL); }
		 | DO '{' State_List '}'
	 				{ lyyerror(@1,"缺少do终止条件");}	 
		 | REPEAT '{' State_List '}' UNTIL '(' Exp ')' ';'
					{ $$=newFlow(REPEAT,$7,$3,NULL); }
		 | REPEAT '{' State_List '}' 
		 			{ lyyerror(@1,"缺少repeat终止条件"); }
		 |	FOR '(' Exp_List ';' Exp ';' Exp_List ')' '{' State_List '}'
		 			{
			  	//cout<<"for catch"<<endl;	
			  		$$ = newFlow(FOR,$5,$10,$7);
			  		if($3!=NULL)
			  		$$ = newAst('L',$3,$$);
					}
		 ;


Exp : NUMBER      		{ $$=newNum($1); }
	| READ '(' ID ')' 	{ $$=newRead($3);}
	| WRITE '(' Exp ')' { $$=newWrite($3);}
	| WRITE '(' STRING ')' { $$=newWrite($3); }
	| EXIT  '(' ')'     { $$=newExit(); }
	| ABS '(' Exp ')'  	{ $$=newAbs( $3 );  } 
	| SQRT '(' Exp ')' 	{ $$=newSqrt( $3 );	}
	| POW '(' Exp ',' Exp ')' { $$=newPow($3,$5); }
	| ODD '(' Exp ')'	{ $$=newOdd($3);	}
	| DEBUG '(' STRING ')'	{ $$=newDebug($3);       }
	| Exp CMP Exp 		{ $$=newCmp($2,$1,$3); }
	| Exp '+' Exp 		{ $$=newAst('+',$1,$3); }
	| Exp '-' Exp 		{ $$=newAst('-',$1,$3); }
	| Exp '*' Exp 		{ $$=newAst('*',$1,$3); }
	| Exp '/' Exp 		{ $$=newAst('/',$1,$3); }
	| DADD    ID  { 
		struct Ast * t1 = newNum(1);
		struct Ast * t2 = newRef($2);
		struct Ast * t3 = newAst('+',t2,t1);
		$$ = newAsgn($2,t3);
		}
	| DSUB    ID  {
   		struct Ast * t1 = newNum(1);
		struct Ast * t2 = newRef($2);
		struct Ast * t3 = newAst('-',t2,t1);
		$$ = newAsgn($2,t3);
		}
	| Exp '%' Exp { $$=newAst('%',$1,$3); }
	| Exp '|' Exp { $$=newAst('|',$1,$3); }
	| Exp '&' Exp { $$=newAst('&',$1,$3); }
	| Exp '^' Exp { $$=newAst('^',$1,$3); }
	|     '~' Exp { $$=newAst('~',$2,NULL); }
	| Exp AND Exp { $$=newAst(AND,$1,$3); }
	| Exp OR  Exp { $$=newAst(OR,$1,$3); }
	|     NOT Exp { $$=newAst(NOT,$2,NULL); }
	| '(' Exp ')' { $$=$2; }
	| '-' Exp %prec UMINUS { $$=newAst('M',$2,NULL); }
	| ID          { $$ = newRef($1);}
	| ID '=' Exp  { $$ = newAsgn($1,$3);}
	;
%%

int main(int argc,char** argv){
	if(init(argc,argv)){
	//	for_a_test();
		yyparse();

	}
	end();
	return 0;
}

void yyerror(char *s,...){
	/*
	va_list ap;
	va_start(ap,s);
	if(yylloc.first_line)
		fprintf(stderr,"%d.%d-%d.%d:error: ",yylloc.first_line-1,
		yylloc.first_column-1,yylloc.last_line-1,yylloc.last_column);
	vfprintf(stderr,s,ap);
	fprintf(stderr,"\n");
	*/
	error_tag = 0;
}

void lyyerror(YYLTYPE t,char * s,...){
	va_list ap;
	va_start(ap,s);
	if(t.first_line)
		fprintf(stderr,"line %d-%d:error:",yylineno-1,yylineno);
	vfprintf(stderr,s,ap);
	fprintf(stderr,"\n");
	error_tag = 0;
}

int init(int argc,char ** argv){
	if(argc==1)
		c_file = stdin;
	else
		c_file = fopen(argv[1],"r");
	if(!c_file)
		   return 0; 	
    yyrestart(c_file);	
	return 1;
}

void end(){
	if(c_file!=stdin)
		fclose(c_file);	
}
