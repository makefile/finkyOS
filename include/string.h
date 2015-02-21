#ifndef _STRING_H
#define _STRING_H
void* memcpy(void* dst,void *src,int size);
void  memset(void* dst,char ch,int size);
void* strcpy(void* dst,void* src);
int strlen(char* str);
void str_rev(char*str);
/**
 * `phys_copy' and `phys_set' are used only in the kernel, where segments
 * are all flat (based on 0). In the meanwhile, currently linear address
 * space is mapped to the identical physical address space. Therefore,
 * a `physical copy' will be as same as a common copy, so does `phys_set'.
 */
#define	phys_copy	memcpy
#define	phys_set	memset

#endif
