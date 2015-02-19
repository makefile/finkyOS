
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              vsprintf.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include"stdio.h"
#include "type.h"
//#include "const.h"
#include "string.h"
#include"display.h"

/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */

/*======================================================================*
                                vsprintf
 *======================================================================*/
int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;
	char	tmp[256];
	va_list	p_next_arg = args;
	int len;
	memset(tmp,0,sizeof(tmp));
	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt) {
		case 'd':
			itoad(tmp, *((int*)p_next_arg));
			strcpy(p, tmp);
			p_next_arg += 4;
			p += strlen(tmp);
			break;

		case 'x':
			itoa(tmp, *((int*)p_next_arg));
			strcpy(p, tmp);
			p_next_arg += 4;
			p += strlen(tmp);
			break;
		case 's':
			strcpy(p,(char*)p_next_arg);
			len=strlen(p_next_arg);
			disp_str(p);
			p_next_arg+=len+1;
			p+=len;
			break;
		default:
			break;
		}
	}

	return (p - buf);
}

