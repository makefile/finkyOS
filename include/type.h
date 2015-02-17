#ifndef _TYPE_H
#define _TYPE_H
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define public 		//space
#define private static  //limit the scope of var in only one file
#define EXTERN extern
typedef void (*irq_handler)(int irq);
#endif
