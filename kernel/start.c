#include"type.h"
#include"const.h"
#include"protect.h"
#include"proto.h"
#include"proc.h"

#include"global.h"

//public void* memcpy(void* pDst,void *pSrc,int size);


public void cstart(){
	
	memcpy(&gdt,(void*)(*((u32*)(&gdt_ptr[2]))),*((u16*)(&gdt_ptr[0]))+1);
	u16* limit=(u16*)gdt_ptr;
	u32* base=(u32*)(&gdt_ptr[2]);
	*limit=DT_NUM*sizeof(DESCRIPTOR)-1;
	*base=(u32)&gdt;
	
	u16* idt_limit=(u16*)idt_ptr;
	u32* idt_base=(u32*)(&idt_ptr[2]);
	*idt_limit=IDT_SIZE*sizeof(GATE)-1;
	*idt_base=(u32)&idt;
	
	//disp_pos=0;
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
		"----------cstart-------\n");
	//disp_int(10);
	
	init_prot();//初始化8259A（屏蔽硬件中断，打开了键盘中断等）始化idt以及gdt中的tss，ldt
	disp_str("----------cend -------\n");
}

