/*
 * idt.h
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */

#ifndef IDT_H_
#define IDT_H_

#include "types.h"
#include "gdt.h"
#include "sys.h"

// x86 idt attr
#define IDT_TYPE_INTERRUPT      0x0E
#define IDT_TYPE_TRAP           0x0F
#define IDT_FLAGS_DPL0          0x00
#define IDT_FLAGS_DPL1          0x20
#define IDT_FLAGS_DPL2          0x40
#define IDT_FLAGS_DPL3          0x60
#define IDT_FLAGS_P             0x80
#define IDT_FLAGS_IST0          0 // use tss's esp0 (legacy method)
#define IDT_FLAGS_IST1          1 // use ist0 in tss

// blue attr
#define IDT_FLAGS_KERNEL        ( IDT_FLAGS_DPL0 | IDT_FLAGS_P )
#define IDT_FLAGS_USER          ( IDT_FLAGS_DPL3 | IDT_FLAGS_P )
#define IDT_MAXENTRYCOUNT       100 // in x86 arch, max IVT(Interrupt Vector Table) is 100

#pragma pack( push, 1 )
typedef struct idt_entry {
	WORD lower_base_addr;
	WORD segment_selector;
	BYTE ist; // IST:3, null:5
	BYTE type_and_flags; // Type:4, null:1, DPL:2, P:1
	WORD middle_base_addr;
	DWORD upper_base_addr;
	DWORD reserved;
} idt_entry_t;

struct {
	idtr_t idtr;
	idt_entry_t entry[IDT_MAXENTRYCOUNT];
} myidt;
#pragma pack ( pop )

typedef void (*irq_handler_t)();

void idt_init(void);
void set_idt_entry(int num, void *handler);

extern void _isr0(void);
extern void _isr1(void);
extern void _isr2(void);
extern void _isr3(void);
extern void _isr4(void);
extern void _isr5(void);
extern void _isr6(void);
extern void _isr7(void);
extern void _isr8(void);
extern void _isr9(void);
extern void _isr10(void);
extern void _isr11(void);
extern void _isr12(void);
extern void _isr13(void);
extern void _isr14(void);
extern void _isr15(void);
extern void _isr16(void);
extern void _isr17(void);
extern void _isr18(void);
extern void _isr19(void);
extern void _isr20(void);
extern void _isr21(void);
extern void _isr22(void);
extern void _isr23(void);
extern void _isr24(void);
extern void _isr25(void);
extern void _isr26(void);
extern void _isr27(void);
extern void _isr28(void);
extern void _isr29(void);
extern void _isr30(void);
extern void _isr31(void);
extern void _isr32(void);
extern void _isr33(void);
extern void _isr34(void);
extern void _isr35(void);
extern void _isr36(void);
extern void _isr37(void);
extern void _isr38(void);
extern void _isr39(void);
extern void _isr40(void);
extern void _isr41(void);
extern void _isr42(void);
extern void _isr43(void);
extern void _isr44(void);
extern void _isr45(void);
extern void _isr46(void);
extern void _isr47(void);
extern void _isr99(void);

#endif /* IDT_H_ */
