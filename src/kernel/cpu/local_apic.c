#include "local_apic.h"
#include "multiprocessor.h"

extern mymachine_t mymachine;

QWORD get_lapic_base_vaddr(void) {
	return mymachine.lapic_base_addr + PAGE_OFFSET;
}

void enable_lapic(void) {
	*(DWORD*) (get_lapic_base_vaddr() + APIC_REGISTER_SVR) |= 0x100;
}

// Send EOI(End of Interrupt) to Local APIC
void irq_ack(void) {
	*(DWORD*) (get_lapic_base_vaddr() + APIC_REGISTER_EOI) = 0;
}

// Initilize LOCAL-APIC
// Set Local-Vector-Table which handles LOCAL-APIC
void local_vector_table_init(void) {
	QWORD memio;

	memio = get_lapic_base_vaddr();

	// Set Task Priority Register to 0
	// LOCAL-APIC can receive all interrupts
	*(DWORD*) (memio + APIC_REGISTER_TASKPRIORITY) = 0;

	// disalbe LOCAL-APIC timer
	*( DWORD*) (memio + APIC_REGISTER_TIMER) |=
	APIC_INTERRUPT_MASK;

	// disable LINT0
	*( DWORD*) (memio + APIC_REGISTER_LINT0) |=
	APIC_INTERRUPT_MASK;

	// Bind LINT1 with NMI interrupt line
	*( DWORD*) (memio + APIC_REGISTER_LINT1) =
	APIC_TRIGGERMODE_EDGE |
	APIC_POLARITY_ACTIVEHIGH | APIC_DELIVERYMODE_NMI;

	// Disable Error Register
	*( DWORD*) (memio + APIC_REGISTER_ERROR) |=
	APIC_INTERRUPT_MASK;

	// Disable PERF_MONITOR_COUNTER
	*( DWORD*) (memio +
	APIC_REGISTER_PERFORMANCEMONITORINGCOUNTER) |= APIC_INTERRUPT_MASK;

	// Disable THERMALSENSOR
	*( DWORD*) (memio + APIC_REGISTER_THERMALSENSOR) |=
	APIC_INTERRUPT_MASK;
}
