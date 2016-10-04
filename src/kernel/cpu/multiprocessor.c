#include "multiprocessor.h"
#include "asm_util.h"
#include "local_apic.h"
#include "pit.h"
#include "jumper.h"

// FLAG only used when booting APs
volatile BYTE ap_on_flag = 0;
volatile int ap_on_count = 0;
volatile QWORD lapic_vaddr = 0;

mymachine_t mymachine;

void machine_init() {
	// in case lenovo, apicid is 0x10, 0x11, 0x12, 0x13
	mymachine.lapic_id[0] = 0x0; // BSP
	mymachine.lapic_id[1] = 0x1; // AP
	mymachine.lapic_id[2] = 0x2; // AP
	mymachine.lapic_id[3] = 0x3; // AP

	mymachine.lapic_base_addr = 0xfee00000;
	mymachine.ioapic_base_vaddr = 0xfec00000;

	/* I/O Interrupt Assign ( VBOX )
	 * name			PIC		IOAPIC	IORemappedNumber
	 * 								(final IRQ num for Processor)
	 * ===============================
	 * keyboard	|	1		1		1
	 * timer	|	0		2		0
	 * 			|	3		3		3
	 * 			|	4		4		4
	 * 			|	5		5		5
	 * 			|	6		6		6
	 * 			|	7		7		7
	 * 			|	8		8		8
	 * 			|	9		9		9
	 * 			|	10		10		10
	 * 			|	11		11		11
	 * 			|	12		12		12
	 * 			|	13		13		13
	 * 			|	14		14		14
	 * 			|	15		15		15
	 */

	//  [cpu used irq num] = ioapic int-in number
	mymachine.final_irq[0] = 2;
	mymachine.final_irq[1] = 1;
	mymachine.final_irq[2] = 255;
	mymachine.final_irq[3] = 3;
	mymachine.final_irq[4] = 4;
	mymachine.final_irq[5] = 5;
	mymachine.final_irq[6] = 6;
	mymachine.final_irq[7] = 7;
	mymachine.final_irq[8] = 8;
	mymachine.final_irq[9] = 9;
	mymachine.final_irq[10] = 10;
	mymachine.final_irq[11] = 11;
	mymachine.final_irq[12] = 12;
	mymachine.final_irq[13] = 13;
	mymachine.final_irq[14] = 14;
	mymachine.final_irq[15] = 15;

	lapic_vaddr = get_lapic_base_vaddr() + APIC_REGISTER_APICID;
}

void smp_init(void) {
	// enable local-apic in all AP
	enable_global_lapic();

	// Enable BSP(Bootstrap Processor) local-apic
	enable_lapic();

	// wake up APs by cpu IPI message
	wakeup_all_ap();

	// If Target Machine use PIC compatible(?) mode, disable PIC mode.
	// so below three codes are optional.
	out_port_b(0x22, 0x70);
	out_port_b(0x23, 0x01);
	mask_pic(0xFFFF);

	// Set details in LOCAL-APIC
	local_vector_table_init();

	// Set redirection table in IO-APIC
	ioapic_redirection_table_init();

	printk("smp? mode [starting]\n");
}

int get_lapicid_from_core_num(int i) {
	return mymachine.lapic_id[i];
}

/* Send two IPI message to other APs by using Lower Interrupt Command Register
 * 1. INIT IPI
 * 2. STARTUP IPI two times
 *    (send one time in Virtual Machine)
 */
void wakeup_all_ap(void) {
	QWORD memio;

	memio = get_lapic_base_vaddr();

	// 1. Send INIT IPI to all other AP
	*(DWORD*) (memio + APIC_REGISTER_ICR_LOWER) =
	APIC_DESTINATIONSHORTHAND_ALLEXCLUDINGSELF | APIC_TRIGGERMODE_EDGE
			| APIC_LEVEL_ASSERT | APIC_DESTINATIONMODE_PHYSICAL
			| APIC_DELIVERYMODE_INIT;

	// copy jumper code to pyhs mem addr 0x10000
	// and set booting flag
	memcpy(0x10000, init_jumper_bin, init_jumper_bin_len);
	ap_on_flag = 1;

	// 2. Send STARTUP IPI
	// 0x10 = 0x10000(AP startup phys addr) / 4KB
	// All Excluding Self, Edge Trigger, Assert, Physical Deenable_intrnation, Start Up
	*(DWORD*) (memio + APIC_REGISTER_ICR_LOWER) =
	APIC_DESTINATIONSHORTHAND_ALLEXCLUDINGSELF | APIC_TRIGGERMODE_EDGE
			| APIC_LEVEL_ASSERT | APIC_DESTINATIONMODE_PHYSICAL
			| APIC_DELIVERYMODE_STARTUP | 0x10;

	while (ap_on_count < (CORE_COUNT - 1))
		;
}

// Get LOCAL-APIC ID
BYTE get_lapic_id(void) {
	QWORD data, memio;

	memio = get_lapic_base_vaddr();
	memio += APIC_REGISTER_APICID;

	data = *(QWORD*) (memio);

	return (data >> 24);
}

BYTE get_core_num(void) { // blue
	int i, id;

	id = get_lapic_id();

	for (i = 0; i < CORE_COUNT; i++) {
		if (mymachine.lapic_id[i] == id) {
			return i;
		}
	}
}
