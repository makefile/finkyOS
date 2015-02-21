#include"time.h"
#include "stdlib.h"
//#include "const.h"
//#include "protect.h"
//#include "tty.h"
#include "console.h"
//#include "string.h"
#include "proc.h"
#include"irq.h"
#include "display.h"
#include"syscall.h"
#include"ipc.h"

int ticks;

int get_ticks(){
//static i=1;
//disp_int(i++);
	MESSAGE msg;
	reset_msg(&msg);
	msg.type=GET_TICKS;
	send_recv(BOTH,TASK_SYS,&msg);
	return msg.RETVAL;
}

//时钟中断处理程序，这里进行进程调度
void clock_handler(int irq){
	ticks++;
	p_proc_ready->ticks--;
	if(k_reenter>=0) return;//,重入，直接返回，跟上面两语句的顺序不应该反了
	if(p_proc_ready->ticks>0) return;//指定的时间片还没运行完，不进行进程切换，这样减少频繁调度。
	
	schedule();
//asm("hlt");
//disp_str(".clock.");
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
