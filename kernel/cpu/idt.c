/*
 * idt.c
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */

#include "idt.h"
#include "interrupt.h"

irq_handler_t isr_routines[IDT_MAXENTRYCOUNT];

void ihandler_init() {
	memset(isr_routines, 0, sizeof(isr_routines));
	memset(&(myidt.entry), 0, sizeof(myidt.entry));

	// not trap gate, just interrupt gate
	// when entering trap gate, IE bit is not changed
	set_idt_entry(0, _isr0);
	set_idt_entry(1, _isr1);
	set_idt_entry(2, _isr2);
	set_idt_entry(3, _isr3);
	set_idt_entry(4, _isr4);
	set_idt_entry(5, _isr5);
	set_idt_entry(6, _isr6);
	set_idt_entry(7, _isr7);
	set_idt_entry(8, _isr8);
	set_idt_entry(9, _isr9);
	set_idt_entry(10, _isr10);
	set_idt_entry(11, _isr11);
	set_idt_entry(12, _isr12);
	set_idt_entry(13, _isr13);
	set_idt_entry(14, _isr14);
	set_idt_entry(15, _isr15);
	set_idt_entry(16, _isr16);
	set_idt_entry(17, _isr17);
	set_idt_entry(18, _isr18);
	set_idt_entry(19, _isr19);
	set_idt_entry(20, _isr20);
	set_idt_entry(21, _isr21);
	set_idt_entry(22, _isr22);
	set_idt_entry(23, _isr23);
	set_idt_entry(24, _isr24);
	set_idt_entry(25, _isr25);
	set_idt_entry(26, _isr26);
	set_idt_entry(27, _isr27);
	set_idt_entry(28, _isr28);
	set_idt_entry(29, _isr29);
	set_idt_entry(30, _isr30);
	set_idt_entry(31, _isr31);
	set_idt_entry(32, _isr32);
	set_idt_entry(33, _isr33);
	set_idt_entry(34, _isr34);
	set_idt_entry(35, _isr35);
	set_idt_entry(36, _isr36);
	set_idt_entry(37, _isr37);
	set_idt_entry(38, _isr38);
	set_idt_entry(39, _isr39);
	set_idt_entry(40, _isr40);
	set_idt_entry(41, _isr41);
	set_idt_entry(42, _isr42);
	set_idt_entry(43, _isr43);
	set_idt_entry(44, _isr44);
	set_idt_entry(45, _isr45);
	set_idt_entry(46, _isr46);
	set_idt_entry(47, _isr47);
	set_idt_entry(IRQ_SYSCALL, _isr99); // not trap gate, this is interrupt gate [Careful]
}

void pit_handler();
void keyboard_handler();
void device_not_avail_handler();
void syscall_handler();

void idt_init(void) {
	myidt.idtr.base_addr = &(myidt.entry);
	myidt.idtr.limit = sizeof(myidt.entry) - 1;

	ihandler_init();
	load_idtr(&(myidt.idtr));

	printk("idt [ok]\n");

	// belows : not appropriate place, need migration
	isr_routines[IRQ_PIT] = pit_handler;
	isr_routines[IRQ_KEYBOARD] = keyboard_handler;
	isr_routines[IRQ_DEVICE_NOT_AVAILABLE] = device_not_avail_handler;

	isr_routines[IRQ_SYSCALL] = syscall_handler;
}

void set_idt_entry(int num, void *handler) {
	myidt.entry[num].lower_base_addr = (QWORD ) handler & 0xFFFF;
	myidt.entry[num].segment_selector = 0x08;
	myidt.entry[num].ist = IDT_FLAGS_IST1 & 0x3;
	myidt.entry[num].type_and_flags = IDT_TYPE_INTERRUPT | IDT_FLAGS_KERNEL;
	myidt.entry[num].middle_base_addr = ((QWORD ) handler >> 16) & 0xFFFF;
	myidt.entry[num].upper_base_addr = (QWORD ) handler >> 32;
	myidt.entry[num].reserved = 0;
}
