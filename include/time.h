#ifndef _TIME_H
#define _TIME_H


extern void clock_handler(int irq);
extern void init_clock();
extern void milli_delay(int m_sec);
extern void sleep(int sec);

#endif
