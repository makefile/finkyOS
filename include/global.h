#ifdef _GLOBAL_VAR
#undef EXTERN
#define EXTERN //space
#endif

EXTERN int disp_pos;
EXTERN u8 gdt_ptr[6];//0-15:limit,16-47:base
EXTERN DESCRIPTOR gdt[DT_NUM];//GDT_SIZE,128
EXTERN u8 idt_ptr[6];//struct same as gdt_ptr
EXTERN GATE idt[IDT_SIZE];
EXTERN TSS tss;
EXTERN PROCESS proc_table[NR_TASKS];
EXTERN PROCESS *p_proc_ready;

//extern	PROCESS		proc_table[];
extern	char		task_stack[];
//extern  TASK            task_table[];

