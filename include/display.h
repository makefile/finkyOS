#ifndef _DISPLAY_H
#define _DISPLAY_H
//disp_clor_str
#define BLACK 	0
#define BLUE	1
#define GREEN	2
#define CYAN	3
#define RED	4
#define YANG	5
#define ZONG	6
#define WHITE	7
#define BRIGHT	8
#define FLASH	0x80
#define MAKE_COLOR(back,front) ((back<<4)|front)

extern int disp_pos;
 
void disp_str(char *info);
void disp_color_str(char *info,int color);
void disp_int(int n);
/* printf.c */
//int     printf(const char *fmt, ...);

/* vsprintf.c */
//int     vsprintf(char *buf, const char *fmt, va_list args);

#endif
