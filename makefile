work: lib.c lib.h small.l small.y
	bison -d small.y 
	flex small.l
	g++ lib.c lib.h small.tab.c small.tab.h  lex.yy.c -std=c++11 -w 




