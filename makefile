##################################################
# Makefile注意：引用为定义变量为空值，错误难以发现，$<指源文件的第一个而不是所有文件
##################################################
AS=nasm
ASFLAGS=-I boot/include/
ASKF=-I include/ -f elf32
CC=gcc
LD=ld
ENTRYPOINT=0x30400
CFLAGS=-I include/ -c -fno-builtin -m32
CF2=-fno-stack-protector
LDFLAGS=-s -Ttext $(ENTRYPOINT)  -m elf_i386
BOOT:=boot/boot.asm
LDR:=boot/loader.asm
KERNEL:=kernel/kernel.asm
OUT:=bin/
BOOT_BIN:=$(OUT)$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(OUT)$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(OUT)$(subst .asm,.bin,$(KERNEL))
OBJS=$(OUT)kernel/kernel.o $(OUT)kernel/global.o $(OUT)kernel/start.o $(OUT)lib/kliba.o $(OUT)lib/string.o $(OUT)lib/cstring.o \
	$(OUT)kernel/protect.o $(OUT)kernel/i8259.o  $(OUT)lib/klib.o \
	$(OUT)kernel/main.o $(OUT)kernel/proc.o  \
	$(OUT)kernel/clock.o $(OUT)kernel/syscall.o \
	$(OUT)kernel/keyboard.o $(OUT)kernel/tty.o \
	$(OUT)kernel/vsprintf.o $(OUT)kernel/printf.o \
	$(OUT)kernel/console.o

IMG:=a.img
MOUNTPOINT:=/mnt/usb/

.PHONY : x all cp clean um img #not file obj
x:bin cp
	@./b
all:clean bin cp
	@ ./b
bin : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
cp:
	@dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc 2> /dev/null
	@sudo mount -o loop $(IMG) $(MOUNTPOINT)
	@sudo cp $(LDR_BIN) $(MOUNTPOINT) #-v
	@sudo cp $(KERNEL_BIN) $(MOUNTPOINT) #-v
	@sleep 0.1
	@sudo umount $(MOUNTPOINT)
	@echo "succ copy to img,run b"
img:
	@dd if=/dev/zero of=$(IMG) bs=512 count=2880
clean :
	rm -f $(OBJS)
$(BOOT_BIN) : $(BOOT) boot/include/load.inc boot/include/fat12hdr.inc
	$(AS) $(ASFLAGS) $< -o $@
$(LDR_BIN) : $(LDR)
	$(AS) $(ASFLAGS) $< -o $@
$(KERNEL_BIN) : $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

bin/kernel/kernel.o : kernel/kernel.asm
	$(AS) $(ASKF) -o $@ $<
bin/kernel/syscall.o: kernel/syscall.asm
	$(AS) $(ASKF) -o $@ $<

bin/kernel/start.o: kernel/start.c include/type.h include/const.h include/protect.h \
		include/proto.h include/string.h
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel/i8259.o : kernel/i8259.c include/type.h include/const.h include/protect.h \
			include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel/global.o : kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel/protect.o : kernel/protect.c
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel/main.o : kernel/main.c
	$(CC) $(CFLAGS) -o $@ $<
bin/kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<
bin/kernel/proc.o: kernel/proc.c
	$(CC) $(CFLAGS) -o $@ $<
bin/kernel/tty.o: kernel/tty.c
	$(CC) $(CFLAGS) -o $@ $<
bin/kernel/console.o: kernel/console.c
	$(CC) $(CFLAGS) -o $@ $<
bin/kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) $(CF2) -o $@ $<
bin/kernel/printf.o: kernel/printf.c
	$(CC) $(CFLAGS) $(CF2) -o $@ $<
bin/kernel/vsprintf.o: kernel/vsprintf.c
	$(CC) $(CFLAGS) $(CF2) -o $@ $<
	
bin/lib/klib.o : lib/klib.c
	$(CC) $(CFLAGS) $(CF2) -o $@ $<

bin/lib/kliba.o : lib/kliba.asm
	$(AS) $(ASKF) -o $@ $<

bin/lib/string.o : lib/string.asm
	$(AS) $(ASKF) -o $@ $<
bin/lib/cstring.o: lib/string.c
	$(CC) $(CFLAGS)  -o $@ $<

um:
	@sudo umount $(MOUNTPOINT)
