#include"string.h"
//#include"display.h"
//#define swap(x,y) do{*(x)+=*(y);*(y)=*(x)-*(y);*(y)=*(x)-*(y);}while(0);
#define swap(x,y) do{int tmp=*(x);*(x)=*(y);*(y)=tmp;}while(0);
/*
void swap(int *x,int *y){
	int tmp=*x;
	*x=*y;
	*y=tmp;
}
*/
void str_rev(char*str){
//	disp_int(str[0]);
//	disp_int(str[1]);disp_int(str[2]);
	int x=strlen(str)/2;
	int i;

//swap(str,str+1);
	for(i=0;i<x;i++)
		swap(str+i,str+x-i);//惊讶地发现如果这里用函数会破坏栈上的数据，目前只能用宏了，但第一个swap宏也不行。
//	disp_str("rev:");disp_str(str);disp_str("...");
//	disp_int(str[2]);
}
