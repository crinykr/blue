/*
 * interrupt.c
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */

#include "interrupt.h"
#include "idt.h"
#include "pic.h"
#include "task.h"
#include "scheduler.h"
#include "util.h"

extern irq_handler_t isr_routines[IDT_MAXENTRYCOUNT];

// Common Interrupt C-Entry Routine
void common_intr_handler(regs_t *r) {
	int irq;
	irq_handler_t handler;

	// dbg
	{
		char tmp_str[30];
		static int tmp_intr_count = 0;
		sprintf(tmp_str, "[%d] interrupt rising : num[%d] core[%d]",
				++tmp_intr_count, r->int_no, get_core_num());
		dbg_console_printxy(0, 0, tmp_str);
	}

	// SAVE CURRENT TASK CONTEXT
	tcb_t *cur_task = get_cur_task(get_core_num());
	memcpy(&(cur_task->context), r, sizeof(regs_t));

	// Call Interrupt Handler
	irq = r->int_no;
	handler = isr_routines[irq];

	if (handler) {
		handler();
	} else {
		no_intr_handler_panic(r);
	}

	// if CPU TRAP
	if (irq < PIC_IRQSTARTVECTOR) {
	}
	// if HARDWARE INTERRUPT
	else if (irq < IRQ_SYSCALL) {
		if (irq != IRQ_PIT) // if not PIT INTR
			intr_balancing(irq); // criny:here
		irq_ack();
	}
	// if SYSTEM CALL
	else {
	}

	// PREPARE NEXT TASK CONTEXT
	cur_task = get_cur_task(get_core_num());
	memcpy(r, &(cur_task->context), sizeof(regs_t));
}

void intr_balancing(int irq) {
	BYTE core_num;
	core_num = get_core_num();
	irq_rout(irq, get_lapicid_from_core_num((core_num + 1) % 4));
}

void no_intr_handler_panic(regs_t *r) {
	printk("**********************************\n");
	printk("** NO HANDLER PANIC : IRQ NUM %d\n", r->int_no);
	printk("**********************************\n");
	print_regs(r);
	_BARRIER_;
}

