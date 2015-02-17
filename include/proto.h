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
public void disable_int(int irq);
public void enable_int(int irq);
public void spurious_irq(int irq);
public void clock_handler(int irq);
public int sys_get_ticks();//proc.c
//syscall.asm
public void sys_call();//int handler
public int get_ticks();
void milli_delay(int m_sec);
void sleep(int sec);

