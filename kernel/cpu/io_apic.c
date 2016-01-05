#include "io_apic.h"
#include "pic.h"
#include "multiprocessor.h"

extern mymachine_t mymachine;

// IO-APIC is bind with ISA BUS
QWORD ioapic_base_vaddr(void) {
	return mymachine.ioapic_base_vaddr + PAGE_OFFSET;
}

void set_ioapic_redirection_entry(io_redirection_table_t* entry, BYTE core,
BYTE intr_mask, BYTE keyflag_and_deliverymode, BYTE vector) {
	memset(entry, 0, sizeof(io_redirection_table_t));

	entry->dest = core;
	entry->keyflag_and_deliverymode = keyflag_and_deliverymode;
	entry->intr_mask = intr_mask;
	entry->vector = vector;
}

void read_ioapic_redirection_table(int intr_in, io_redirection_table_t* entry) {
	QWORD* data;
	QWORD memio;

	memio = ioapic_base_vaddr();
	data = (QWORD*) entry;

	*(DWORD*) (memio + IOAPIC_REGISTER_IOREGISTERSELECTOR) =
	IOAPIC_REGISTERINDEX_HIGHIO_REDIRECTION_TABLE + intr_in * 2;
	*data = *(DWORD*) (memio + IOAPIC_REGISTER_IOWINDOW);
	*data = *data << 32;

	*(DWORD*) (memio + IOAPIC_REGISTER_IOREGISTERSELECTOR) =
	IOAPIC_REGISTERINDEX_LOWIO_REDIRECTION_TABLE + intr_in * 2;
	*data |= *(DWORD*) (memio + IOAPIC_REGISTER_IOWINDOW);
}

void write_ioapic_redirection_table(int intr_in, io_redirection_table_t* entry) {
	QWORD* data;
	QWORD memio;

	memio = ioapic_base_vaddr();
	data = (QWORD*) entry;

	*(DWORD*) (memio + IOAPIC_REGISTER_IOREGISTERSELECTOR) =
	IOAPIC_REGISTERINDEX_HIGHIO_REDIRECTION_TABLE + intr_in * 2;
	*(DWORD*) (memio + IOAPIC_REGISTER_IOWINDOW) = *data >> 32;

	*(DWORD*) (memio + IOAPIC_REGISTER_IOREGISTERSELECTOR) =
	IOAPIC_REGISTERINDEX_LOWIO_REDIRECTION_TABLE + intr_in * 2;
	*(DWORD*) (memio + IOAPIC_REGISTER_IOWINDOW) = *data;
}

void mask_all_intr_ioapic(void) {
	io_redirection_table_t stEntry;
	int i;

	for (i = 0; i < IOAPIC_MAXIO_REDIRECTION_TABLECOUNT; i++) {
		read_ioapic_redirection_table(i, &stEntry);
		stEntry.intr_mask = IOAPIC_INTERRUPT_MASK;
		write_ioapic_redirection_table(i, &stEntry);
	}
}

void ioapic_redirection_table_init(void) {
	int i, final_irq;
	BYTE dest;
	io_redirection_table_t entry;

	mask_all_intr_ioapic();

	for (final_irq = 0; final_irq < IOAPIC_MAXIRQTOINTINMAPCOUNT; final_irq++) {

		if (final_irq == 2)
			continue;

		if (final_irq == 0) {
			// PIT binds to all LOCAL-APIC
			dest = 0xFF;
		} else {
			// first, others are bind to BP LOCAL-APIC
			dest = get_lapic_id();
		}

		set_ioapic_redirection_entry(&entry, dest, 0x00,
				IOAPIC_TRIGGERMODE_EDGE | IOAPIC_POLARITY_ACTIVEHIGH
						| IOAPIC_DESTINATIONMODE_PHYSICALMODE
						| IOAPIC_DELIVERYMODE_FIXED,
				PIC_IRQSTARTVECTOR + final_irq);

		write_ioapic_redirection_table(mymachine.final_irq[final_irq], &entry);
	}
}

// Caution : PIT re-rout is forbidden in blue
void irq_rout(int irq, BYTE local_apic_id) { // criny
	int irq_vector;
	io_redirection_table_t stEntry;

	irq_vector = irq - PIC_IRQSTARTVECTOR;

	read_ioapic_redirection_table(mymachine.final_irq[irq_vector], &stEntry);
	stEntry.dest = local_apic_id;
	write_ioapic_redirection_table(mymachine.final_irq[irq_vector], &stEntry);
}

