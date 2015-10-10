/*
 * prime.cs
*/

var i,j,flag,str;
i = 2;
write('******** 1到100的素数********');
while(i <= 100){
	j = 2;
	while(j<i and (i mod j != 0 )) { ++j;}
	if(i==2 or j==i){
		write(i);
	}
	++i;
}
