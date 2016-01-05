#ifndef IOAPIC_H_
#define IOAPIC_H_

#include "types.h"
#include "sys.h"

#define IOAPIC_REGISTER_IOREGISTERSELECTOR              0x00
#define IOAPIC_REGISTER_IOWINDOW                        0x10
#define IOAPIC_REGISTERINDEX_IOAPICID                   0x00
#define IOAPIC_REGISTERINDEX_IOAPICVERSION              0x01
#define IOAPIC_REGISTERINDEX_IOAPICARBID                0x02
#define IOAPIC_REGISTERINDEX_LOWIO_REDIRECTION_TABLE    0x10
#define IOAPIC_REGISTERINDEX_HIGHIO_REDIRECTION_TABLE   0x11

#define IOAPIC_MAXIO_REDIRECTION_TABLECOUNT             24 // max machine-defined size#define IOAPIC_MAXIRQTOINTINMAPCOUNT                    16 // real used size to match with legacy pic
#define IOAPIC_INTERRUPT_MASK                           0x01

// Trigger Mode
#define IOAPIC_TRIGGERMODE_LEVEL                        0x80
#define IOAPIC_TRIGGERMODE_EDGE                         0x00

// Remote IRR
#define IOAPIC_REMOTEIRR_EOI                            0x40
#define IOAPIC_REMOTEIRR_ACCEPT                         0x00

// Interrupt Input Pin Polarity
#define IOAPIC_POLARITY_ACTIVELOW                       0x20
#define IOAPIC_POLARITY_ACTIVEHIGH                      0x00

// Delivery Status
#define IOAPIC_DELIFVERYSTATUS_SENDPENDING              0x10
#define IOAPIC_DELIFVERYSTATUS_IDLE                     0x00

// Deenable_intrnation Mode
#define IOAPIC_DESTINATIONMODE_LOGICALMODE              0x08
#define IOAPIC_DESTINATIONMODE_PHYSICALMODE             0x00

// Delivery Mode
#define IOAPIC_DELIVERYMODE_FIXED                       0x00
#define IOAPIC_DELIVERYMODE_LOWESTPRIORITY              0x01
#define IOAPIC_DELIVERYMODE_SMI                         0x02
#define IOAPIC_DELIVERYMODE_NMI                         0x04
#define IOAPIC_DELIVERYMODE_INIT                        0x05
#define IOAPIC_DELIVERYMODE_EXTINT                      0x07

#pragma pack( push, 1 )
typedef struct {
	BYTE vector;
	BYTE keyflag_and_deliverymode;
	BYTE intr_mask;
	BYTE reserved[4];
	BYTE dest; // destination local apic id
} io_redirection_table_t;
#pragma pack( pop )

QWORD ioapic_base_vaddr(void);
void set_ioapic_redirection_entry(io_redirection_table_t* entry, BYTE core,
BYTE intr_mask, BYTE keyflag_and_deliverymode, BYTE vector);
void read_ioapic_redirection_table(int intr_in, io_redirection_table_t* entry);
void write_ioapic_redirection_table(int intr_in, io_redirection_table_t* entry);
void mask_all_intr_ioapic(void);
void ioapic_redirection_table_init(void);
void irq_rout(int iIRQ, BYTE core);

#endif /* IOAPIC_H_ */
