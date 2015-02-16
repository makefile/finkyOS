//全局的变量和函数声明都在这，这个文件的意义就是链接器链接之前实例话所有的这些。
#define _GLOBAL_VAR 
#include"type.h"
#include"const.h"
#include"protect.h"
#include"proto.h"
#include"proc.h"
#include"global.h"
//disp_pos in global.h,在使用disp_str（disp_int)之前必须初始化这个变量

public char task_stack[STACK_SIZE_TOTAL];

