
#include "type.h"
//#include "const.h"
//#include "protect.h"
//#include"tty.h"
//#include"console.h"
//#include "proto.h"
//#include "string.h"
//#include "global.h"
#include"display.h"
#include"stdlib.h"
#include"string.h" //str_rev
/*======================================================================*
                               itoa
 *======================================================================*/
/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
public char * itoa(char * str, int num)
{
	char *	p = str;
	char	ch;
	int	i;
	int	flag = 0;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = 1;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}
/*my func,has some problem
char* itoad(char* str,int num){
	char* p=str;
	int left;
	if(num==0){ *p++='0';*p=0;return str;}
	else if(num<0){
		num=-num;
		*p++='-';
	}
	while(num){
		left=num%10;
		*p++='0'+left;
		num/=10;
	}
	*p=0;
	if(str[0]!='-') str_rev(str);
	else str_rev(&str[1]);
	return str;
}
*/
/*======================================================================*
                               disp_int
 *======================================================================*/
public void disp_int(int input)
{
	//disp_str("start disp_int\n");
	char output[16];
	itoa(output, input);
	disp_str(output);
}
public void delay(int time){
	int i,k;
	for(k=0;k<time;k++)
		for(i=0;i<1000000;i++);
}
