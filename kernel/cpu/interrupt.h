/*
 * interrupt.h
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "types.h"
#include "sys.h"

// cpu interrupt
#define IRQ_DIVIDE_ERROR 0
#define IRQ_DEBUG 1
#define IRQ_NMI 2
#define IRQ_BREAK_POINT 3
#define IRQ_OVER_FLOW 4
#define IRQ_BOUND_RANGE_EXCCEDED 5
#define IRQ_INVALID_OPCODE 6
#define IRQ_DEVICE_NOT_AVAILABLE 7
#define IRQ_DOUBLE_FAULT 8
#define IRQ_COPROCESSOR_SEGMENT_OVERRUN 9
#define IRQ_INVALID_TSS 10
#define IRQ_SEGMENT_NOT_PPRESENT 11
#define IRQ_STACK_SEGMENT_FAULT 12
#define IRQ_GENERAL_PROTECTION 13
#define IRQ_PAGE_FAULT 14
#define IRQ_TRAP_RESERVED 15
#define IRQ_FPU_ERROR 16
#define IRQ_ALIGNMENT_CHECK 17
#define IRQ_MACHINE_CHECK 18
#define IRQ_SIMD_ERROR 19
#define IRQ_ETC_EXCEPTION 20

// hardware interrupt
#define IRQ_PIT 32
#define IRQ_KEYBOARD 33
#define IRQ_SLAVE_PIC 34
#define IRQ_SERIAL2 35
#define IRQ_SERIAL1 36
#define IRQ_PARALLEL2 37
#define IRQ_FLOPPY 38
#define IRQ_PARALLER1 39
#define IRQ_RTC 40
#define IRQ_RESERVED 41
#define IRQ_NOTUSED1 42
#define IRQ_NOTUSED2 43
#define IRQ_MOUSE 44
#define IRQ_COPROCESSOR 45
#define IRQ_HDD1 46
#define IRQ_HDD2 47

// system call software interrupt
#define IRQ_SYSCALL 99

// max interrupt vector count in blue
// 16 is the count of ISA BUS interrupt
#define INTR_MAX_COUNT 16

void common_intr_handler(regs_t *r);
void intr_balancing(int irq);
void no_intr_handler_panic(regs_t *r);

#endif /* INTERRUPT_H_ */
