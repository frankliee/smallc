#include "lib.h"
#include "small.tab.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <unordered_map> 
#include <set>
#include <vector>
#include <memory>
#include <string.h>
#include <string>
#include <iostream>
#include <math.h>
using namespace std;
map<string,Symbol*> table;
set< char* > str_buffer;
void table_init(){
	table.clear();
}

void table_show(){
	cout<<"***table of symbols**"<<endl;
	cout<<"size:"<<table.size()<<endl;
	for(auto i = table.begin();i!=table.end();i++){
		if(i->second->func==NULL)
			cout<<i->second->name<<"("<<i->second->value<<") ";
		else
			cout<<i->second->name<<" ";
	}
	cout<<endl<<"*********************"<<endl;
}

void var_show(){
	for(auto i = table.begin();i!=table.end();i++)
		if(i->second->func==NULL)
			cout<<i->second->name<<"("<<i->second->value<<") ";
	cout<<endl;
}

void var_show(string name){
	if(table.find(name)==table.end() && table[name]->func==NULL )
		cout<<"unkown var"<<endl;
	else
		cout<<table[name]->value<<endl;
}

void for_a_test(){
	int token;
	while(token=yylex()){
	//	cout<<yytext<<endl;
		switch(token){
			case 'L': cout<<"列表"<<endl;break;
			case IF : cout<<"if"<<endl;break;
			case WHILE: cout<<"while"<<endl;break;
			case NUMBER: cout<<"number value:"<<yylval.d<<endl;break;
			case READ: cout<<"read"<<endl;break;
			case WRITE: cout<<"write"<<endl;break;
			case EXIT: cout<<"exit"<<endl;break;
			case DADD: cout<<"++"<<endl;break;
			case VAR: cout<<"var"<<endl;break;
			case DSUB: cout<<"--"<<endl;break;
			case '%': case '|':case '&':case '^':case '~':
			case '.': case ',':case ':':case ';':case '(':case ')':
			case '[': case ']':case '{':case '}':
			case '+': case '-':case '*':case '/':
					   cout<<(char)token<<endl;break;
			case AND: cout<<"and"<<endl;break;
			case OR : cout<<"or"<<endl;break;
			case NOT : cout<<"not"<<endl;break;
			case ID  : cout<<"ID name:"<<yylval.sym->name<<endl;break;
			case '=': cout<<"="<<endl;break;
			case CMP: cout<<"CMP type:"<<yylval.type<<endl;break;
			case 'M': cout<<"负号"<<endl;break;
			case ABS: cout<<"abs"<<endl;break;
			case SQRT: cout<<"sqrt"<<endl;break;
			case POW: cout<<"pow"<<endl;break;
			default: cout<<"unkown token value:"<<token<<endl;
		}
	
	}
}

struct Symbol *
lookup(char * name){
	string str(name);
	if(table.find(str)==table.end()){
		struct Symbol * sym = new Symbol();
		sym->name = str;
		sym->value = 0;
		sym->func = NULL;
		sym->symlist = NULL;
		table[str]=sym;
	}
	return table[str];
}

struct Ast * 
newStr(char * str){
	struct String * ast = new String();
	if(!ast){
		yyerror("out of space\n");
		exit(0);
	}
	ast->type = STRING;
	int len  = strlen(str);
	ast->str = "";
	for(auto i=1;i<=len - 2;i++)
			(ast->str).push_back(str[i]);
	return (struct Ast *)ast;
}

struct Ast *
newAst(int type,struct Ast * l,struct Ast * r){
	struct Ast * ast = new Ast();
	if(!ast){
		yyerror("out of space\n");
		exit(0);
	}
	ast->type = type;
	ast->l = l;
	ast->r = r;
	return ast;
}

struct SymList *
newSymList(struct Symbol * sym){
	struct SymList * symList = new SymList();
	symList->l.push_back(sym);
	return symList;
}

struct SymList *
insertSymList(char c,struct Symbol * sym,struct SymList * symList){
	if(c == 'b')
		symList->l.push_back(sym);
	else if(c == 'f')
		symList->l.push_front(sym);
	return symList;
}


struct Ast * 
newRead(struct Symbol * sym){
	struct Ast * ast = new Ast();
	ast->type = READ;
	ast->l = (struct Ast *)sym;
	return ast;
}

struct Ast * 
newWrite(struct Ast * value){
	struct Ast * ast = new Ast();
	ast->type = WRITE;
	ast->l = value;
	return ast;
}

struct Ast *
newWrite(char * str){
	struct Ast * ast = new Ast();
	ast->type = WRITE;
	ast->l = newStr(str);
	return ast;
}

struct Ast *
newExit(){
	struct Ast * ast = new Ast();
	ast->type = EXIT;
	return ast;
};

struct Ast *
newAbs(struct Ast * value ){
	struct Ast * ast = new Ast();
	ast->type = ABS;
	ast->l = value;
	return ast;
}

struct Ast *
newSqrt(struct Ast * value){
	struct Ast * ast = new Ast();
	ast->type = SQRT;
	ast->l = value;
	return ast;
}

struct Ast *
newPow(struct Ast * a,struct Ast * b){
	struct Ast * ast = new Ast();
	ast->type = POW;
	ast->l = a;
	ast->r = b;
	return ast;
}

struct Ast *
newOdd(struct Ast * value){
	struct Ast * ast = new Ast();
	ast->type = ODD;
	ast->l = value;
	return ast;
}

struct Ast *
newDebug(char * str){
	struct Ast * debug = new Ast();
	debug->type = DEBUG;
	debug->l = newStr(str);
	debug->r = NULL;
	return debug;
}

struct Ast *
newNum(double d){
	struct Numval * num = new Numval();
	if(!num){
		yyerror("out of space\n");
		exit(0);
	}
	num->type = NUMBER;
	num->value = d;
	return (struct Ast *)num;	
}

struct Ast *
newCmp(int ctype,struct Ast * l,struct Ast * r){
	struct Ast * ast = new Ast();
	if(!ast){
		yyerror("out of space\n");
		exit(0);
	}
	ast->type = '0'+ctype;
	ast->l = l;
	ast->r = r;
	return ast;
}
/*
struct Ast *
newBCall(int btype,struct Ast * arg){
	struct BCall * call = new BCall();
	if(!call){
		yyerror("out of space\n");
		exit(0);
	}
	call->type = 'F';
	call->arg = arg;
	call->btype = btype;
	return (struct Ast *)call;
}

struct Ast *
newCall(struct Symbol * sym,struct Ast * arg){
	struct UCall * call = new UCall();
	if(!call){
		yyerror("out of space\n");
		exit(0);
	}
	call->type = 'C';
	call->sym  = sym;
	call->arg = arg;
	return (struct Ast*)call;
}
*/
struct Ast *
newRef(struct Symbol * sym){
	struct SymRef * ref = new SymRef();
	if(!ref){
		yyerror("out of space\n");
		exit(0);
	}
	ref->type = ID;
	ref->sym = sym;
	return (struct Ast *)ref;
}

struct Ast *
newAsgn(struct Symbol * sym,struct Ast * l){
	struct SymAsgn * symA = new SymAsgn();
	if(!symA){
		yyerror("out of space\n");	
		exit(0);
	}
	symA->type = '=';
	symA->sym = sym;
	symA->value = l;	
	return (struct Ast *)symA;
}

struct Ast * 
newFlow(int type,struct Ast * cond,struct Ast * tl,struct Ast * el){
	struct Flow * flow = new Flow();
    if(!flow){
		yyerror("out of space\n");
		exit(0);
	}	
	flow->type = type;
	flow->cond = cond;
	flow->tl = tl;
	flow->el = el;
	return (struct Ast *)flow;
}

void
treefree(struct Ast * root){
	switch(root->type){
		case NUMBER:case ID:
			break;
		
		case '=':
			treefree(((struct SymAsgn *)root)->value);
			break;

		case '+':case '-':case '*':case '/':
		case 'M':case '%':case '|':case '&':
		case '^':case '~':
		case '1':case '2':case '3':case '4':case '5':case '6':
		case AND:case OR :case NOT:
		case READ:case WRITE:case EXIT:case ABS:case SQRT:
		case POW :case ODD  :case DEBUG:case 'L':
			if(root->l!=NULL)
				treefree(root->l);
			if(root->r!=NULL)
				treefree(root->r);
			break;

		case IF:case WHILE:
			treefree(((struct Flow *)root)->cond);
			if(((struct Flow *)root)->tl != NULL)
				treefree(((struct Flow *)root)->tl);
			if(((struct Flow *)root)->el !=NULL)
				treefree(((struct Flow *)root)->el);
			break;
	}
	free(root);
}

void tablefree(){
	for(auto i=table.begin();i!=table.end();i++)
		free(i->second);		
	for(auto i=str_buffer.begin();i!=str_buffer.end();i++)
		free(*i);
}

double 
eval(struct Ast * ast){
	double v;
	string cmd;
	if(!ast){
		yyerror("interl error: null eval");
		return 0.0;
	}
	switch(ast->type){
		/*  常量  */
	case NUMBER : v = ((struct Numval *)ast)->value; break;
		/*  名字引用 */ 
	case ID : v = ((struct SymRef *)ast)->sym->value; break;
		/* 赋值 */
	case '=': v = ((struct SymAsgn *)ast)->sym->value
			  	= eval(((struct SymAsgn *)ast)->value); break;
		/* 表达式 */
	case '+': v = eval(ast->l) + eval(ast->r);          break;
	case '-': v = eval(ast->l) - eval(ast->r);          break;
	case '*': v = eval(ast->l) * eval(ast->r);          break;
	case '/': v = eval(ast->l) / eval(ast->r);          break;
	case 'M': v =-eval(ast->l)				 ;          break;
	case '%': v = (int)eval(ast->l) % (int)eval(ast->r);break;
	case '|': v = (int)eval(ast->l) | (int)eval(ast->r);break;
	case '&': v = (int)eval(ast->l) & (int)eval(ast->r);break;
	case '^': v = (int)eval(ast->l) ^ (int)eval(ast->r);break;
	case '~': v =~(int)eval(ast->l);                    break;
		  /* 比较 */
	case '1': v = (eval(ast->l) > eval(ast->r)) ? 1:0; break;
	case '2': v = (eval(ast->l) < eval(ast->r)) ? 1:0; break;
	case '3': v = (eval(ast->l) != eval(ast->r))? 1:0; break;
	case '4': v = (eval(ast->l) == eval(ast->r))? 1:0; break;
	case '5': v = (eval(ast->l) >= eval(ast->r))? 1:0; break;
	case '6': v = (eval(ast->l) <= eval(ast->r))? 1:0; break;
		 /* 布尔 */
	case AND: if(eval(ast->l)>=1 && eval(ast->r)>=1)
					v = 1;
			  else
					v = 0;
			  break;
	case OR: if(eval(ast->l)>=1 || eval(ast->r)>=1)
					v = 1;
			  else
					v = 0;
			  break;
	case NOT: if(eval(ast->l)>=1)
					v = 0;
			  else
					v = 1;
			  break;
		/* 控制流 */
		/* if/then/else */
	case IF:  
		if(eval(((struct Flow *)ast)->cond)!=0){  /* 判断条件 */
			if(((struct Flow *)ast)->tl)
				v = eval(((struct Flow *)ast)->tl);
			else
				v = 0.0;
		}else{
			if(((struct Flow *)ast)->el)
				v = eval(((struct Flow *)ast)->el);
			else
				v = 0.0;
		}
		break;
		/* while */
	case WHILE:
		v = 0.0;
		if(((struct Flow *)ast)->tl){
			while(eval(((struct Flow *)ast)->cond)!=0)
				v = eval(((struct Flow *)ast)->tl);
		}
		break;
		/* for */
	case FOR:
		v = 0.0;
		if(((struct Flow *)ast)->tl){
			while(eval(((struct Flow *)ast)->cond)!=0){
				v = eval(((struct Flow *)ast)->tl);
				if(((struct Flow *)ast)->el)
					eval(((struct Flow *)ast)->el);
			}
		}
		break;
		/* 内建函数 */
		/* Read */
	case READ:
	   	v = 0.0;
		cout<<"In -> ";
	   	cin>>((struct Symbol *)(ast->l))->value;
	   	break;
		/* Write */
	case WRITE:
	   v = 0.0;
	  if(ast->l->type != STRING)
	  		cout<<"Out : "<<eval(ast->l)<<endl; 
	  else
		    cout<<((struct String *)(ast->l))->str<<endl;
	   break;
		/* Exit */
	case EXIT:cout<<"exit"<<endl; exit(0); break;
		/* Abs */
	case ABS: 
		v = eval(ast->l);
		v = v < 0 ? -v : v;
		break;
		/* Sqrt */
	case SQRT:
		v = sqrt(eval(ast->l));
		break;
		/* Pow */
	case POW:
		v = pow(eval(ast->l),eval(ast->r));
		break;
		/* Odd */
	case ODD:
		v = eval(ast->l);
		v = (int) v % 2;
		break;
		/* Debug */
	case DEBUG: 
		v = 0.0;
		cout<<"断点:"<<((struct String*)(ast->l))->str;
		cout<<"(跳过s,查看全部变量pa,查看变量 p 变量名)"<<endl;
		while(cin>>cmd && cmd!="s"){
			if(cmd == "pa"){
				var_show();
			}
			else if(cmd == "p"){
				cin>>cmd;
				var_show(cmd);
			}
		}
		break;
		/* 列表语句 */
	case 'L':
		eval(ast->l);
		v = eval(ast->r);
		break;
		/*  调用内建函数 */
		/* 调用用户自定义函数 */
	default:
		printf("internal error: bad node %c\n",ast->type);
		break;
	}
	return v;
}

void print(struct Ast * node){
	cout<<"print "<<eval(node)<<endl;
}
/*
double callBuiltin(struct BCall * call){
	int v = 0.0;
	return v;
}

double callUser(struct UCall * call){
	int v = 0.0;
	return v;
}
*/

