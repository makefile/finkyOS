
#ifndef _STDLIB_H
#define _STDLIB_H
#include"type.h"
extern void out_byte(u16 port,u8 value);
extern unsigned char in_byte(u16 port);
extern char* itoa(char *str,int n);//integer to 16-based %x
extern char* itoad(char *str,int n);//integer  %d

#endif
