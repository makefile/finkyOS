#ifndef _PROC_H
#define _PROC_H
#include"ipc.h"
#include"protect.h" //descriptor
/* Number of tasks */
#define NR_TASKS 4		//number of task.ring1
#define NR_PROCS 3	//ring3
#define LDT_SIZE 2	//每个任务（进程）一个单独的LDT(内含两项cs,ds)

typedef struct s_stackframe {	/* proc_ptr points here				↑ Low			*/
	u32	gs;		/* ┓						│			*/
	u32	fs;		/* ┃						│			*/
	u32	es;		/* ┃						│			*/
	u32	ds;		/* ┃						│			*/
	u32	edi;		/* ┃						│			*/
	u32	esi;		/* ┣ pushed by save()				│			*/
	u32	ebp;		/* ┃						│			*/
	u32	kernel_esp;	/* <- 'popad' will ignore it			│			*/
	u32	ebx;		/* ┃						↑栈从高地址往低地址增长*/		
	u32	edx;		/* ┃						│			*/
	u32	ecx;		/* ┃						│			*/
	u32	eax;		/* ┛						│			*/
	//u32	retaddr;	/* return address for assembly code save()	│			*/
	u32	eip;		/*  ┓						│			*/
	u32	cs;		/*  ┃						│			*/
	u32	eflags;		/*  ┣ these are pushed by CPU during interrupt	│			*/
	u32	esp;		/*  ┃						│			*/
	u32	ss;		/*  ┛						┷High			*/
}STACK_FRAME;


typedef struct proc {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */

        int ticks;                 /* remained ticks */
        int priority;

	u32 pid;                   /* process id passed in from MM */
	char name[16];           /* name of the process */
	int nr_tty;	//关联的TTY
	int  p_flags;              /**
				    * process flags.
				    * A proc is runnable iff p_flags==0
				    */

	MESSAGE * p_msg;
	int p_recvfrom;
	int p_sendto;

	int has_int_msg;           /**
				    * nonzero if an INTERRUPT occurred when
				    * the task is not ready to deal with it.
				    */

	struct proc * q_sending;   /**
				    * queue of procs sending messages to
				    * this proc
				    */
	struct proc * next_sending;/**
				    * next proc in the sending
				    * queue (q_sending)
				    */
}PROCESS;
typedef  void (*task_f)();
typedef struct s_task {
	task_f	initial_eip;
	int	stacksize;
	char	name[32];
}TASK;

#define TASK_TTY	0 //pid
#define TASK_SYS	1
#define proc2pid(x) (x - proc_table)
#define FIRST_PROC	proc_table[0]
#define LAST_PROC	proc_table[NR_TASKS + NR_PROCS - 1]
/* stacks of tasks */
#define STACK_SIZE_TTY  	0x8000
#define STACK_SIZE_SYS  	0x8000
#define STACK_SIZE_HD	  	0x8000
#define STACK_SIZE_FS	  	0x8000
#define STACK_SIZE_PROCA	0x8000 //32kB
#define STACK_SIZE_PROCB	0x8000
#define STACK_SIZE_PROCC	0x8000
//极为重要，不可有偏差
#define STACK_SIZE_TOTAL	(STACK_SIZE_TTY+ \
				STACK_SIZE_SYS   + \
				STACK_SIZE_HD+STACK_SIZE_FS+ \
				STACK_SIZE_PROCA + \
				STACK_SIZE_PROCB + \
				STACK_SIZE_PROCC)
extern void procA();
extern void procB();
extern void procC();

extern void schedule();

extern PROCESS proc_table[];
extern PROCESS *p_proc_ready;
extern	char		task_stack[];
extern  TASK            task_table[];
extern TASK user_task_table[];

extern void task_tty();
extern void task_sys();
extern	void*	va2la(int pid, void* va);
#define INDEX_LDT_RW 1
extern	int	ldt_seg_linear(struct proc* p, int idx);
extern	void	reset_msg(MESSAGE* p);
extern	void	dump_msg(const char * title, MESSAGE* m);
extern	void	dump_proc(struct proc * p);
extern	int	send_recv(int function, int src_dest, MESSAGE* msg);
#endif
