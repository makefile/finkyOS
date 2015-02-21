
%include"sconst.inc"
;SELECTOR_KERNEL_CS equ 8 ;01000b,右移3位变为1,即第1个选择子（第0个CPU保留不用）
;SELECTOR_TSS equ 0x20 ;从外层跳到内层时ss：esp的值从TSS获得
;导入函数/全局变量,在start.c中
extern cstart
extern	exception_handler
;void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags);
;中断或异常发生时eflags，cs，eip已被压栈，如果有错误，错误码也已被压栈
;C调用约定是调用者恢复堆栈
extern spurious_irq
extern kernel_main
extern disp_str
extern delay
extern clock_handler

extern gdt_ptr
extern idt_ptr
extern disp_pos 
extern p_proc_ready
extern tss
extern k_reenter
extern irq_table
extern sys_call_table

[section .data]
clock_msg db "*"

[section .bss] ;未初始化数据区
StackSpace resb 2*1024
StackTop:	;栈顶

[section .text]	; 代码在此

global _start	; 导出 _start
global restart ;main.c kernel_main中调用，开始进程
;异常处理函数
global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
;中断处理函数
global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15

global sys_call

_start:	; 跳到这里来的时候，我们假设 gs 指向显存

	;esp现在仍指向Loader中，现需指向kernel中，方便使用，谓之切换堆栈
	mov esp,StackTop ;让堆栈在bss段中
	mov dword [disp_pos],0 ;第一次初始化全局变量disp_pos,当然你也可以在c代码中初始化，否则可能是随机值
	sgdt [gdt_ptr] ;保存GdtPtr到u8 gdt_ptr[6]这个数组中,供cstart()使用
	call cstart ;此函数改变gdt_ptr,指向新GDT
	lgdt [gdt_ptr] ;使用新GDT
	lidt [idt_ptr]
	
	jmp SELECTOR_KERNEL_CS:csinit ;强制使用刚初始化的结构,第一个选择子为SelectorFlatC
	; CS的数值在保护模式下不能直接更改，但可利用jmp等方法改，SelectorFlatC的基地址为0,所以cs=0,0:csinit就跳到了下一句，这个jmp的意义就是给cs赋新值
csinit:
	;jmp 0x30:0
	;ud2
	;push 0
	;popfd ;pop top of stack into EFLAGS，let EFLAGS=0
	
;	mov	ah, 1dh ;pink/blue	; 0000: 黑底    1111: 白字
;	mov	al, 'K'
;	mov	[gs:((80 * 5 + 39) * 2)], ax	; 屏幕第 5 行, 第 39 列
	;sti;这里不必要了，上面cstart中的init_8259A已经开启了一部分硬件中断
	xor eax,eax
	mov ax,SELECTOR_TSS
	ltr ax
	
	jmp kernel_main

restart: ;在main.c中被调用
	
	mov esp,[p_proc_ready] ;esp指向PCB的栈顶
	lldt [esp+P_LDT_SEL]
	;惊喜啊，将lldt抄写成了lidt,导致bochs总是重启系统，其中日志显示3rd exception with no resolution,在OSDev Wiki中讲到可能是IDT内容或描述符出错
	lea eax,[esp+P_STACKTOP]
	mov dword [tss+TSS3_S_SP0],eax
	;jmp re_enter
re_enter: ;让重入的中断直接返回
	dec dword [k_reenter] 
;	hlt        
        pop gs
        pop fs
        pop es
        pop ds
        popad
        iretd
; 中断和异常 -- 硬件中断
; ---------------------------------
%macro  hwint_master    1
	pushad ;a,c,d,b,esp,ebp,esi,edi
        push ds
        push es
        push fs
        push gs ;为了不影响进程的执行，保存原寄存器，以防万一
        ;因为mov al,EOI改变了al的值
        mov dx,ss
        mov ds,dx ;为了使用调用函数等
        mov es,dx
        mov fs,dx     
	in al,INT_M_CTLMASK
	or al,(1<<%1)
	out INT_M_CTLMASK,al ;不容许再发生该中断,也就是不会再发生重入，那重入的代码也就没用了
	
        mov al,EOI ;reenable master 8259
        out INT_M_CTL,al ;将EOI位置1,告诉8259A当前中断结束，否则不再发生时钟中断
        mov esp,StackTop ;切换到内核栈

        sti ;CPU在响应中断的过程中会自动关中断，下面的操作可能很耗时，应该容许其他中断的发生，如键盘响应等。
        ;call clock_handler
        push %1
        call [irq_table+4 * %1]
        pop ecx
        cli
       
        in al,INT_M_CTLMASK
	and al,~(1<<%1)
	out INT_M_CTLMASK,al ;容许再发生该中断
	
	inc dword [k_reenter]
        cmp dword [k_reenter],0
        jne re_enter
        jmp restart
	
%endmacro
; ---------------------------------

ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
        ;inc byte [gs:14] ;第一行第5个字符跳动变化
	hwint_master	0
	
ALIGN   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15

; 中断和异常 -- 异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 相当于弹出两个，让栈顶指向 EIP，
				;堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt
sys_call:
	pushad ;a,c,d,b,esp,ebp,esi,edi
        push ds
        push es
        push fs
        push gs ;为了不影响进程的执行，保存原寄存器，以防万一
        ;因为mov al,EOI改变了al的值
        mov esi,edx ;保存edx，系统调用的第四个是参数
        mov dx,ss
        mov ds,dx ;这三个为了使用调用函数等
        mov es,dx
        mov fs,dx
        mov edx,esi ;恢复edx
        mov esi ,esp
        mov esp,StackTop ;切换到内核栈
        ;save over
        inc dword [k_reenter]
        cmp dword [k_reenter],0
        jne re_enter
        
        push esi ;sys_write调用者指针
        push dword [p_proc_ready]
        push edx
        push ecx
        push ebx
        
;        sti
        call [sys_call_table+eax*4] ;系统调用的代码要在内核栈中运行
        add esp,16 ;4*4
        pop esi
        mov [esi+EAXREG-P_STACKBASE],eax ;return val
        
;        cli
       
        
        jmp restart
        
