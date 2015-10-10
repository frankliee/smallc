/* 
* gcd.cs
*/

var a,b,t,m;
write('输入a和b，输出最大公倍数');
write('若a和b均为0，退出程序');
while(true){
	read(a);
	read(b);
	m = a * b;
	if(a==0 and b==0) { exit(); } /*a,b均为0，退出*/
	if(a < b) { t = a; a = b; b = t; } /* a<b 时交换a,b */
	while(b != 0){
		//debug(s1); /* 调试功能 */ 
		t = a mod b;
		a = b;
		b = t;
	}
	write(m / a);
}

