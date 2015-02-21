
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include"display.h"
#include"tty.h"
/* CONSOLE */
typedef struct s_console
{
	unsigned int	current_start_addr;	/* 当前显示到了什么位置	  */
	unsigned int	original_addr;		/* 当前控制台对应显存位置 */
	unsigned int	v_mem_limit;		/* 当前控制台占的显存大小 */
	unsigned int	cursor;			/* 当前光标位置 */
}CONSOLE;

#define SCR_UP	1	/* scroll forward */
#define SCR_DN	-1	/* scroll backward */

#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80
#define SCR_WIDTH SCREEN_WIDTH
#define DEFAULT_CHAR_COLOR	0x07	/* 0000 0111 黑底白字 */
#define GRAY_CHAR (MAKE_COLOR(BLACK,BLACK)|BRIGHT)
#define RED_CHAR (MAKE_COLOR(BLUE,RED)|BRIGHT)
#define NR_CONSOLES	3 //tty
/* VGA */
#define	CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	0xC	/* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	0xD	/* reg index of video mem start addr (LSB) */
#define	CURSOR_H	0xE	/* reg index of cursor position (MSB) */
#define	CURSOR_L	0xF	/* reg index of cursor position (LSB) */
#define	V_MEM_BASE	0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	0x8000	/* 32K: B8000H -> BFFFFH */

extern CONSOLE console_table[NR_CONSOLES];
extern int nr_current_console;
/* console.c */
extern void out_char(CONSOLE* p_con, char ch);
extern void init_screen(TTY* p_tty);
extern void scroll_screen(CONSOLE* p_con, int direction);
extern int is_current_console(CONSOLE* p_con);
extern void out_str(CONSOLE* p_con, char* str);
extern void select_console(int nr_console);
#endif /* _CONSOLE_H_ */
