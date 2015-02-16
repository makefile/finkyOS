;
;
; 清屏
%macro clearScreen 0
mov	ax, 0600h		; AH = 6,  AL = 0h
mov	bx, 0700h		; 黑底白字(BL = 07h)
mov	cx, 0			; 左上角: (0, 0)
mov	dx, 0184fh		; 右下角: (80, 50)//80列25行，每个字符2byte
int	10h			; int 10h
%endmacro



