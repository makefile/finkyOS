
#ifndef _SYSCALL_H
#define _SYSCALL_H
/* system call */
#define NR_SYS_CALL     3
typedef  void* system_call;
extern system_call sys_call_table[];
/* syscall.asm */
extern void    sys_call();             /* int_handler */

/* 以下是系统调用相关 */
/* 系统调用 - 系统级 */
/* proc.c */
//int     sys_get_ticks();
extern int     sys_write(char* buf, int len, PROCESS* p_proc);
extern int     sys_sendrec(int function, int src_dest, MESSAGE* m, PROCESS* p);
extern int     sys_printx(int _unused1, int _unused2, char* s, PROCESS * p_proc);
/* 系统调用 - 用户级 */
extern  int     get_ticks();
extern  void    write(char* buf, int len);
extern  int     sendrec(int function, int src_dest, MESSAGE* p_msg);
extern  int     printx(char* str);
#endif
