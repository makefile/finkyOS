
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

public int kernel_main(){
	
	disp_str("start kernel_main----\n");
	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;//now first proc
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],//change DPL
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		//选择子，后三位为101,TIL为1,表示32位寻址
		//思考良久，突然洞悟，给cs赋值为第0选择子，ds等第1选择子，选择的是LDT表中的。
		//上面刚memcpy的代码段和数据段选择子，实际代表相同内存区域，
		//但当jmp selector:offset时cs的权限不同，CPU会在跳转之前检查权限（代码段只读，防止你写破坏）。
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;//后三位111

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 15;
	proc_table[1].ticks = proc_table[1].priority =  5;
	proc_table[2].ticks = proc_table[2].priority =  3;

	put_irq_handler(CLOCK_IRQ,clock_handler);//设置时钟中断处理程序
	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;
//	init_clock();
//        init_keyboard();
	//disp_str("--------------------------------\n");
	restart();
	
	while(1);//not reach here
}
void procA(){
	int i=0;
	//disp_pos=0;
		while(1){
			disp_int(get_ticks());
			disp_str("A");
			disp_int(i++);
			disp_str("|");
			delay(1);
		}
}
void procB(){
	int i=0x100;
		while(1){
			disp_str("B");
			disp_int(i++);
			disp_str("|");
			delay(2);
		}
}
void procC(){
	int i=0x1000;
		while(1){
			disp_str("C");
			disp_int(i++);
			disp_str("|");
			delay(3);
		}
}
