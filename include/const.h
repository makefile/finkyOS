#ifndef _CONST_H_
#define _CONST_H_

#define DT_NUM 128 	//the descriptor num in GDT and IDT
#define IDT_SIZE 256	//IDT number

#define NR_TASKS 1	//number of process
#define LDT_SIZE NR_TASKS*2	//每个任务（进程）一个单独的LDT(内含两项cs,ds)
/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller       <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller<Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints <Slave>  */
#endif
