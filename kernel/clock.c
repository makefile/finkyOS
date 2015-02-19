#include"time.h"
#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include"irq.h"
#include "global.h"
#include "display.h"
#include "proto.h"
void schedule(){
	p_proc_ready++;
	if(p_proc_ready>=proc_table+NR_TASKS+NR_PROCS)
		p_proc_ready=proc_table;
}
int sys_get_ticks(){
	return ticks;
}
//时钟中断处理程序，这里进行进程调度
void clock_handler(int irq){
	ticks++;
	schedule();
}
void init_clock(){
	//init 8253可编程中断定时器
	out_byte(TIMER_MODE ,RATE_GENERATOR);
	out_byte(TIMER0,(u8)(TIMER_FREQ/HZ));
	out_byte(TIMER0 ,(u8)(TIMER_FREQ/HZ));
	put_irq_handler(CLOCK_IRQ,clock_handler);//设置时钟中断处理程序
}
void milli_delay(int m_sec){
	int t=get_ticks();
	while(((get_ticks()-t)*1000/HZ)<m_sec);
}
void sleep(int sec){
	while(sec--) milli_delay(1000);
}
