#ifdef _GLOBAL_VAR
#undef EXTERN
#define EXTERN //space,多次一举，extern即可
#endif

#ifndef EXTERN
#include"type.h" //.c文件可单独包含本文件而不用再包含type.h
#endif
EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6];//0-15:limit,16-47:base
EXTERN DESCRIPTOR gdt[DT_NUM];//GDT_SIZE,128
EXTERN u8 idt_ptr[6];//struct same as gdt_ptr
EXTERN GATE idt[IDT_SIZE];
EXTERN TSS tss;
EXTERN PROCESS proc_table[NR_TASKS+NR_PROCS];
EXTERN PROCESS *p_proc_ready;

//extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK            task_table[];
extern irq_handler irq_table[];
EXTERN int k_reenter;
EXTERN int ticks;
EXTERN TTY tty_table[NR_CONSOLES];
EXTERN CONSOLE console_table[NR_CONSOLES];
EXTERN int nr_current_console;
