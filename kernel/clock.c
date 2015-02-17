#include"type.h"
#include"const.h"
#include"protect.h"
#include"proto.h"
#include"proc.h"
#include"global.h"
//始终中断处理程序，这里进行进程调度
public void clock_handler(int irq){
	ticks++;
	p_proc_ready++;
	if(p_proc_ready>=proc_table+NR_TASKS)
		p_proc_ready=proc_table;
}
public void milli_delay(int m_sec){
	int t=get_ticks();
	while(((get_ticks()-t)*1000/HZ)<m_sec);
}
void sleep(int sec){
	while(sec--) milli_delay(1000);
}
