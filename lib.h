#ifndef LIB_H
#define LIB_H
#include <list>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#define mydebug 1
using namespace std;

/* flex 定义的函数和变量 */
extern int yylex();
extern char * yytext;
extern int yylineno;
extern void yyerror(char *s,...);
/* ---------------------------------*/

/*  简单测试函数  */

void for_a_test();

/*  抽象语法树节点  */
struct Ast{
	int type;
	struct Ast * l = NULL;
	struct Ast * r = NULL;
};

struct AstList {
	list<struct Ast *> l;
};

/*    标识=变量/函数        */
struct Symbol{
	int type;
	string name;
	double value;
	char * str;
	struct Ast * func = NULL;   /* 函数体 */
	struct SymList* symlist = NULL; /*虚拟参数*/	
	bool ret;
};

/*      标识符列表           */
struct SymList{
	list< struct Symbol* > l;
	void print(){
		if(mydebug==1)
		for(auto i=l.begin();i!=l.end();i++)
			cout<<"name:"<<(*i)->name<<" value:"<<(*i)->value<<endl;
	}
};


/*     调用内建的函数     
struct BCall{
	int type;
	struct Ast * arg;
	int  btype;
};

    调用用户的自定义函数        
struct UCall{
	int type;
	struct Ast * arg;  
	struct Symbol * sym; 
};
*/

/*      流控制     */
struct Flow{
	int type;
	struct Ast * cond = NULL;
	struct Ast *  tl = NULL;
	struct Ast *  el = NULL;
};

/*      数值       */
struct Numval{
	int type;
	double value;
};

/* 字符串 */
struct String{
	int type ;
	string  str;
};

/*    变量引用     */
struct SymRef{
	int type;
	struct Symbol * sym = NULL;
};	

/*    赋值     */
struct SymAsgn{
	int type;
	struct Symbol * sym = NULL;
	struct Ast * value = NULL;
};

struct Call{
	int type;
	struct Symbol * call;
	struct Ast * l;
};

/*  初始化符号表 */
void 
table_init();

/* 显示符号表，调试使用  */
void table_show();

/* 显示变量 */
void var_show();
void var_show(string name);

/*  查找标识符，若无则添加  */
struct Symbol * 
lookup(char *  name);

/* 创建字符串 */
struct Ast * newStr(char * str);

/* 创建新的抽象树节点 */
struct Ast *
newAst(int type,struct Ast * l,struct Ast * r);

/* */
struct AstList *
newAstList(struct Ast * ast);
/* */
struct AstList *
insertAstList(char c,struct Ast * ast, struct AstList * astList);

/*  创建新的符号列表 */
struct SymList *
newSymList(struct Symbol * sym);

/* 向符号列表中添加符号 */
struct SymList *
insertSymList(char c,struct Symbol * sym,struct SymList * symList);

/* 创建阅读节点 */
struct Ast * 
newRead(struct Symbol * sym);

/* 创建打印节点 */
struct Ast *
newWrite(struct Ast * value);

struct Ast *
newWrite(char * str);

/* 创建退出节点 */
struct Ast *
newExit();

/* 创建绝对值节点 */
struct Ast *
newAbs(struct Ast * value);

/* 创建开方节点 */
struct Ast *
newSqrt(struct Ast * value);

/* 创建幂函数节点 */
struct Ast *
newPow(struct Ast * a , struct Ast * b);

/* 创建开方节点 */
struct Ast *
newSqrt();

/* 创建奇偶判断节点 */
struct Ast *
newOdd(struct Ast * ast);

/* 创建调试节点 */
struct Ast *
newDebug(char * str);

/* 创建比较节点   */
struct Ast *
newCmp(int ctype,struct Ast * l,struct Ast * r);

/* 创建内建函数调的动作 */
struct Ast *
newBCall(int btype,struct Ast * l);

/* 创建用户自定义函数调用动作 */
struct Ast *
newCall(struct Symbol * sym,struct Ast * l);

/* 创建新的引用获取动作 */
struct Ast *
newRef(struct Symbol * sym);

/*  创建新的赋值动作 */
struct Ast *
newAsgn(struct Symbol * sym,struct Ast * l);

/* 创建新的数值动作 */
struct Ast *
newNum(double d);

/* 创建新的流控制动作 */
struct Ast *
newFlow(int type, struct Ast * cond, struct Ast * tl,struct Ast * el);

struct Ast *
newCall(struct Symbol * fuc, struct Ast * l);

/*  定义函数  */
void 
def(struct Symbol *  name,struct SymList * symlist,struct Ast * st, bool ret);

double
doCall(struct Call * call);

/*  调用内建函数  */
//double callBuiltin(struct BCall * call);

/* 调用用户函数 */
//double callUser(struct UCall * call);

/*   计算抽象语法树的值  */
double eval(struct Ast * l);

/*  打印节点值  */
void print(struct Ast * node);

/*  删除树   */
void treefree(struct Ast * root);

/*  删除表  */

void tablefree();
/* --------------------*/


#endif

