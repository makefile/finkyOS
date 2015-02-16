

[SECTION .data]
;disp_pos	dd	0
extern disp_pos ;in global.h
[SECTION .text]

; 导出函数
global	disp_str
global	disp_color_str
global out_byte
global in_byte
; ========================================================================
;                  void disp_str(char * info);
; ========================================================================
disp_str:
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pInfo
	mov	edi, [disp_pos] ;edi=ds:disp_pos
	mov	ah, 0Fh ;黑底亮白
.1:
	lodsb
	test	al, al
	jz	.2 ;al=0即'\0'
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2 ;一个字符用两个字节(ax),占2个position
	jmp	.1

.2:
	mov	[disp_pos], edi ;记住最后一次打印时的位置（下一位置）

	pop	ebp
	ret

disp_color_str: ;(str,int color)
	push	ebp
	mov	ebp, esp

	mov	esi, [ebp + 8]	; pStr
	mov	edi, [disp_pos] ;edi=ds:disp_pos
	mov	ah,  [ebp+12] ;黑底亮白
.1:
	lodsb
	test	al, al
	jz	.2 ;al=0即'\0'
	cmp	al, 0Ah	; 是回车吗?
	jnz	.3
	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1
.3:
	mov	[gs:edi], ax
	add	edi, 2 ;一个字符用两个字节(ax),占2个position
	jmp	.1

.2:
	mov	[disp_pos], edi ;记住最后一次打印时的位置（下一位置）

	pop	ebp
	ret
; ========================================================================
;                  void out_byte(u16 port, u8 value);
; ========================================================================
out_byte:
	mov	edx, [esp + 4]		; port
	mov	al, [esp + 4 + 4]	; value
	out	dx, al
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  u8 in_byte(u16 port);
; ========================================================================
in_byte:
	mov	edx, [esp + 4]		; port
	xor	eax, eax
	in	al, dx
	nop	; 一点延迟
	nop
	ret
