/*
 * sys.h
 *
 *  Created on: 2014. 8. 19.
 *      Author: criny
 */

#ifndef SYS_H_
#define SYS_H_

#include "types.h"

extern void *img_end;

extern void* IST_CORE0;
extern void* EARLY_PGT;
extern void* STACK_CORE0;

#define PAGE_OFFSET 0xffff800000000000

typedef struct {
	QWORD gs, fs, es, ds;
	QWORD r15, r14, r13, r12, r11, r10, r9, r8;
	QWORD rsi, rdi, rdx, rcx, rbx, rax, rbp;
	QWORD int_no, err_code;
	QWORD rip, cs, rflags, rsp, ss;
} regs_t;

#endif /* SYS_H_ */
