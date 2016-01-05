#ifndef MULTIPROCESSOR_H_
#define MULTIPROCESSOR_H_

#include "types.h"
#include "io_apic.h"

void smp_init(void);
void machine_init(void);
int get_lapicid_from_core_num(int i);
void wakeup_all_ap(void);
BYTE get_lapic_id(void);
BYTE get_core_num(void);

#define CORE_COUNT 4

typedef struct {
	BYTE lapic_id[CORE_COUNT];
	BYTE final_irq[IOAPIC_MAXIRQTOINTINMAPCOUNT];
	QWORD lapic_base_addr;
	QWORD ioapic_base_vaddr;
} mymachine_t;

#endif /* MULTIPROCESSOR_H_ */
