
#include "type.h"
#include "const.h"
//#include "protect.h"
//#include "tty.h"
#include"irq.h" //k_reenter,ticks
#include "console.h"
#include "string.h"
#include "proc.h"
//#include "global.h"
#include "display.h"
//#include "proto.h"
public int kernel_main(){
	
//	disp_str("start kernel_main----\n");
	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;//now first proc
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u8 privilege;
	u8 rpl;
	int eflags;
	int i;
	int prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
		if(i<NR_TASKS){
			p_task=task_table+i;
			privilege=PRIVILEGE_TASK;
			rpl=RPL_TASK;
			eflags=0x1202;
			
			prio=15;
		}else{//user proc
			p_task=user_task_table+i-NR_TASKS;
			privilege=PRIVILEGE_USER;
			rpl=RPL_USER;
			eflags=0x202;//剥夺用户进程所有的IO权限
			
			prio=5;
		}	
		strcpy(p_proc->name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],//change DPL
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		//选择子，后三位为101,TIL为1,表示32位寻址
		//思考良久，突然洞悟，给cs赋值为第0选择子，ds等第1选择子，选择的是LDT表中的。
		//上面刚memcpy的代码段和数据段选择子，实际代表相同内存区域，
		//但当jmp selector:offset时cs的权限不同，CPU会在跳转之前检查权限（代码段只读，防止你写破坏）。
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| rpl;//后三位111

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;//栈用来供进程压栈用，目前每个仅分配32K
		p_proc->regs.eflags = eflags; /* IF=1, IOPL=1 */
		
		p_proc->nr_tty=0;
		
		p_proc->p_flags=0;
		p_proc->p_msg=0;
		p_proc->p_recvfrom=NO_TASK;
		p_proc->p_sendto=NO_TASK;
		p_proc->has_int_msg=0;
		p_proc->q_sending=0;
		p_proc->next_sending=0;
		
		p_proc->ticks=p_proc->priority=prio;
		
		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[NR_TASKS+0].nr_tty=0;
	proc_table[NR_TASKS+1].nr_tty=1;
	proc_table[NR_TASKS+2].nr_tty=1;
	k_reenter = 0;
	ticks = 0;


	p_proc_ready	= &proc_table[0];
	init_clock();
//	init_keyboard(); //tty.c中调用
	disp_str("-----------call restart()------------\n");
	restart();
	
	while(1);//not reach here
}


void procA(){
	int i;
	//disp_pos=0;
		while(1){
			//disp_color_str("A",BRIGHT|MAKE_COLOR(BLACK,GREEN));
			//printf("A ticks:%d    ",get_ticks());//
			//delay(1);
			//milli_delay(1000);
			sleep(1);
		}
}
void procB(){
	//int i=0x100;
		while(1){
			//disp_color_str("B ",BRIGHT|MAKE_COLOR(BLACK,GREEN));
			//printf("B: do you? \n");
			//disp_str("|");
			sleep(1);
		}
}
void procC(){
	//int i=0x1000;
		while(1){
			//disp_color_str("C ",BRIGHT|MAKE_COLOR(BLACK,GREEN));
			printf("C: Umm...(sleep) \n");
			//disp_str("|");
			sleep(2);
		}
}
