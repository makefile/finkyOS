%include "sconst.inc"

_NR_sendrec equ 0 
_NR_write	equ 1
_NR_printx equ 2

INT_VECTOR_SYS_CALL equ 0x82

global printx ;
global write
global sendrec

bits 32
[section .text]

printx:
	mov eax,_NR_printx
	mov edx,[esp+4]
	int INT_VECTOR_SYS_CALL
	ret
write:
	mov eax,_NR_write
	mov ebx,[esp+4]
	mov ecx,[esp+8]
	int INT_VECTOR_SYS_CALL
	ret
sendrec: ;user should call send_recv() instead
	mov eax,_NR_sendrec
	mov ebx,[esp+4] ;function
	mov ecx,[esp+8] ;src_dest
	mov edx,[esp+12] ;msg
	int INT_VECTOR_SYS_CALL
	ret
	
