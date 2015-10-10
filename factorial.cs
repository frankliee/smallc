/* 
* factorial.cs
*/
var i,n,t;
write( '输入n，输出n的阶乘n!' );
write( '若n为负数，退出程序' );
while(true){
	i = t = 1;
	read(n); 
    if(n<0){ exit(); } /* n 小于0 则退出 */	
	while(i<=n){       /* n 大于等于0，则计算阶乘 */
		t = t * i;
		++i;
	}
	write(t);  /*输出阶乘*/
}
