
#ifndef _STDIO_H
#define _STDIO_H
#include "type.h"
#define STR_DEFAULT_LEN 1024
/* printf.c */
int     printf(const char *fmt, ...);
#define printl printf
/* vsprintf.c */
int     vsprintf(char *buf, const char *fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...);

#endif
