//函数原型
public void out_byte(u16 port,u8 value);
public u8 in_byte(u16 port);
public void disp_str(char *info);
public void disp_color_str(char *info,int color);
public void disp_int(int n);
public void init_prot();
public void init_8259A();
public void put_irq_handler(int irq,irq_handler handler);
public void disable_irq(int irq);
public void enable_irq(int irq);
public void disable_int();
public void enable_int();
public void spurious_irq(int irq);
public void clock_handler(int irq);
public int sys_get_ticks();//proc.c
//syscall.asm
public void sys_call();//int handler
public int get_ticks();
void milli_delay(int m_sec);
void sleep(int sec);
void init_clock();
void init_keyboard();
/* tty.c */
public void task_tty();
public void in_process(TTY* p_tty, u32 key);

/* console.c */
public void out_char(CONSOLE* p_con, char ch);
public void scroll_screen(CONSOLE* p_con, int direction);

/* printf.c */
public  int     printf(const char *fmt, ...);

/* vsprintf.c */
public  int     vsprintf(char *buf, const char *fmt, va_list args);

/* 以下是系统调用相关 */

/* 系统调用 - 系统级 */
/* proc.c */
public  int     sys_get_ticks();
public  int     sys_write(char* buf, int len, PROCESS* p_proc);
/* syscall.asm */
public  void    sys_call();             /* int_handler */

/* 系统调用 - 用户级 */
public  int     get_ticks();
public  void    write(char* buf, int len);


