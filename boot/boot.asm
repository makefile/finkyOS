
;%define	_BOOT_DEBUG_	; 做 Boot Sector 时一定将此行注释掉!将此行打开后用 nasm Boot.asm -o Boot.com 做成一个.COM文件易于调试

%ifdef	_BOOT_DEBUG_
	org  0100h			; 调试状态, 做成 .COM 文件, 可调试
%else
	org  07c00h			; Boot 状态, Bios 将把 Boot Sector 加载到 0:7C00 处并开始执行
%endif

;================================================================================================
%ifdef	_BOOT_DEBUG_
BaseOfStack		equ	0100h	; 调试状态下堆栈基地址(栈底, 从这个位置向低地址生长)
%else
BaseOfStack		equ	07c00h	; Boot状态下堆栈基地址(栈底, 从这个位置向低地址生长)
%endif
;================================================================================================

	
%include	"display_util.inc" ;macro function
%include 	"load.inc" ;const

	jmp short LABEL_START		; Start to boot.
	nop				; 这个 nop 不可少
; 下面是 FAT12 磁盘的头, 之所以包含它是因为下面用到了磁盘的一些信息
%include	"fat12hdr.inc" ;const,我发现这些数据貌似只能放在jmp LABEL_START后边，数据区的最后？

LABEL_START:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack

	; 清屏
	clearScreen ;display_util.inc

	mov	dh, 0			; "Booting  "
	call	DispStr			; 显示字符串
	
	xor	ah, ah	; ┓
	xor	dl, dl	; ┣ 软驱复位
	int	13h	; ┛
	
; 下面在 A 盘的根目录寻找 LOADER.BIN
	mov	word [wSectorNo], SectorNoOfRootDirectory
LABEL_SEARCH_IN_ROOT_DIR_BEGIN:
	cmp	word [wRootDirSizeForLoop], 0	; ┓
	jz	LABEL_NO_LOADERBIN		; ┣ 判断根目录区是不是已经读完
	dec	word [wRootDirSizeForLoop]	; ┛ 如果读完表示没有找到 LOADER.BIN
	mov	ax, BaseOfLoader
	mov	es, ax			; es <- BaseOfLoader
	mov	bx, OffsetOfLoader	; bx <- OffsetOfLoader	于是, es:bx = BaseOfLoader:OffsetOfLoader,下面call ReadSector时将根目录某扇区的内容读到此处，找到loader后，loader也放在这里
	mov	ax, [wSectorNo]	; ax <- Root Directory 中的某 Sector 号
	mov	cl, 1
;	从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
	call	ReadSector

	mov	si, LoaderFileName	; ds:si -> "LOADER  BIN"
	mov	di, OffsetOfLoader	; es:di -> BaseOfLoader:0100 = BaseOfLoader*10h+100
	cld
	mov	dx, 10h
LABEL_SEARCH_FOR_LOADERBIN:
	cmp	dx, 0										; ┓循环次数控制,
	jz	LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR	; ┣如果已经读完了一个 Sector,
	dec	dx											; ┛就跳到下一个 Sector
	mov	cx, 11
LABEL_CMP_FILENAME:
	cmp	cx, 0
	jz	LABEL_FILENAME_FOUND	; 如果比较了 11 个字符都相等, 表示找到
dec	cx
	lodsb				; ds:si -> al
	cmp	al, byte [es:di]
	jz	LABEL_GO_ON
	jmp	LABEL_DIFFERENT		; 只要发现不一样的字符就表明本 DirectoryEntry 不是
; 我们要找的 LOADER.BIN
LABEL_GO_ON:
	inc	di
	jmp	LABEL_CMP_FILENAME	;	继续循环

LABEL_DIFFERENT:
	and	di, 0FFE0h						; else ┓	di &= E0 为了让它指向本条目开头
	add	di, 20h							;     ┃
	mov	si, LoaderFileName					;     ┣ di += 20h  下一个目录条目
	jmp	LABEL_SEARCH_FOR_LOADERBIN;    ┛

LABEL_GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add	word [wSectorNo], 1
	jmp	LABEL_SEARCH_IN_ROOT_DIR_BEGIN

LABEL_NO_LOADERBIN:
	mov	dh, 2			; "No LOADER."
	call	DispStr			; 显示字符串
%ifdef	_BOOT_DEBUG_
	mov	ax, 4c00h		; ┓
	int	21h			; ┛没有找到 LOADER.BIN, 回到 DOS
%else
	jmp	$			; 没有找到 LOADER.BIN, 死循环在这里
%endif

LABEL_FILENAME_FOUND:			; 找到 LOADER.BIN 后便来到这里继续
;此时es:di指向的存放根目录项的内存区域的文件名末尾，一个条目为32byte
	mov	ax, RootDirSectors
	and	di, 0FFE0h		; di -> 当前条目的开始
	add	di, 01Ah		; di -> 首 Sector，根目录条目偏移量0x1A的地方为DIR_FstClus,即文件的FAT表项的第一簇的序号，此处为扇区号，条目的格式及其中的内容应当是在你将磁盘置为FAT12格式被操作系统识别后再往上放文件时，操作系统帮你完成的
	mov	cx, word [es:di]
	push	cx			; 保存此 Sector 在 FAT 中的序号
	add	cx, ax
	add	cx, DeltaSectorNo	; 这句完成时 cl 里面变成 LOADER.BIN 的起始扇区号 (从 0 开始数的序号)
	mov	ax, BaseOfLoader
	mov	es, ax			; es <- BaseOfLoader
	mov	bx, OffsetOfLoader	; bx <- OffsetOfLoader	于是, es:bx = BaseOfLoader:OffsetOfLoader = BaseOfLoader * 10h + OffsetOfLoader
	mov	ax, cx			; ax <- Sector 号

LABEL_GOON_LOADING_FILE:
	push	ax			; ┓
	push	bx			; ┃
	mov	ah, 0Eh			; ┃ 每读一个扇区就在 "Booting  " 后面打一个点, 形成这样的效果:
	mov	al, '.'			; ┃
	mov	bl, 0Fh			; ┃ Booting ......
	int	10h			; ┃
	pop	bx			; ┃
	pop	ax			; ┛

	mov	cl, 1
	call	ReadSector
	pop	ax			; 取出此 Sector 在 FAT 中的序号
	call	GetFATEntry
	cmp	ax, 0FFFh
	jz	LABEL_FILE_LOADED
	push	ax			; 保存 Sector 在 FAT 中的序号
	mov	dx, RootDirSectors
	add	ax, dx
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jmp	LABEL_GOON_LOADING_FILE
LABEL_FILE_LOADED:

	mov	dh, 1			; "Ready."
	call	DispStr			; 显示字符串

; *****************************************************************************************************
	jmp	BaseOfLoader:OffsetOfLoader	; 这一句正式跳转到已加载到内存中的 LOADER.BIN 的开始处
						; 开始执行 LOADER.BIN 的代码
						; Boot Sector 的使命到此结束
; *****************************************************************************************************



;============================================================================
;变量
;----------------------------------------------------------------------------
wRootDirSizeForLoop	dw	RootDirSectors	; Root Directory 占用的扇区数, 在循环中会递减至零.
wSectorNo		dw	0		; 要读取的扇区号
bOdd			db	0		; 奇数还是偶数

;============================================================================
;字符串
;----------------------------------------------------------------------------
LoaderFileName		db	"LOADER  BIN", 0	; LOADER.BIN 之文件名
; 为简化代码, 下面每个字符串的长度均为 MessageLength
MessageLength		equ	9
BootMessage:		db	"Booting  "; 9字节, 不够则用空格补齐. 序号 0
Message1		db	"Ready.   "; 9字节, 不够则用空格补齐. 序号 1
Message2		db	"No LOADER"; 9字节, 不够则用空格补齐. 序号 2
;============================================================================

;----------------------------------------------------------------------------
; 函数名: DispStr
;----------------------------------------------------------------------------
; 作用:
;	显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
DispStr:
	mov	ax, MessageLength
	mul	dh
	add	ax, BootMessage
	mov	bp, ax			; ┓
	mov	ax, ds			; ┣ ES:BP = 串地址
	mov	es, ax			; ┛
	mov	cx, MessageLength	; CX = 串长度
	mov	ax, 01301h		; AH = 13,  AL = 01h
	mov	bx, 0002h		; 页号为0(BH = 0) 黑底白字(BL = 07h),2为绿色
	mov	dl, 0
	int	10h			; int 10h
	ret
%include	"load_util.inc"

times 	510-($-$$)	db	0	; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55				; 结束标志
