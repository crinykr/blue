#include "gdt.h"
#include "util.h"
#include "multiprocessor.h"
#include "fault.h"
#include "interrupt.h"
#include "sys.h"

// GDTR, GDT, TSS
void gdt_tss_init(void) {
	// gdtr
	mygdt.gdtr.limit = sizeof(mygdt.entries) - 1;
	mygdt.gdtr.base_addr = &(mygdt.entries);

	// gdt
	set_gdt_entry8(&(mygdt.entries.null), 0, 0, 0, 0, 0);

	// gdt - kernel code, data
	set_gdt_entry8(&(mygdt.entries.kernel_code), 0, 0xFFFFF,
	GDT_FLAGS_UPPER_CODE,
	GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE);
	set_gdt_entry8(&(mygdt.entries.kernel_data), 0, 0xFFFFF,
	GDT_FLAGS_UPPER_DATA,
	GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA);

	// gdt - user data, code
	set_gdt_entry8(&(mygdt.entries.user_data), 0, 0xFFFFF,
	GDT_FLAGS_UPPER_DATA,
	GDT_FLAGS_LOWER_USERDATA, GDT_TYPE_DATA);
	set_gdt_entry8(&(mygdt.entries.user_code), 0, 0xFFFFF,
	GDT_FLAGS_UPPER_CODE,
	GDT_FLAGS_LOWER_USERCODE, GDT_TYPE_CODE);

	// gdt - kernel tss
	int i;
	for (i = 0; i < CORE_COUNT; i++)
		set_gdt_entry16(&(mygdt.entries.tss[i]), &mygdt.tss[i],
				sizeof(tss_t) - 1,
				GDT_FLAGS_UPPER_TSS,
				GDT_FLAGS_LOWER_TSS, GDT_TYPE_TSS);

	// tss
	tss_init();

	// final init
	load_gdtr(&mygdt.gdtr);
	load_tr(GDT_TSSSEGMENT);

	printk("gdt, tss [ok]\n");
}

void set_gdt_entry8(gdt_entry8_t* entry, DWORD base_addr, DWORD limit,
BYTE upper_flags, BYTE lower_flags, BYTE type) {
	entry->lower_limit = limit & 0xFFFF;
	entry->lower_base_addr = base_addr & 0xFFFF;
	entry->upper_base_addr1 = (base_addr >> 16) & 0xFF;
	entry->type_and_lower_flag = lower_flags | type;
	entry->upper_limit_and_upper_flag = ((limit >> 16) & 0xFF) | upper_flags;
	entry->upper_base_addr2 = (base_addr >> 24) & 0xFF;
}

void set_gdt_entry16(gdt_entry16_t* entry, QWORD base_addr, DWORD limit,
BYTE upper_flags, BYTE lower_flags, BYTE type) {
	entry->lower_limit = limit & 0xFFFF;
	entry->lower_base_addr = base_addr & 0xFFFF;
	entry->middle_base_addr1 = (base_addr >> 16) & 0xFF;
	entry->type_and_lower_flag = lower_flags | type;
	entry->upper_limit_and_upper_flag = ((limit >> 16) & 0xFF) | upper_flags;
	entry->middle_base_addr2 = (base_addr >> 24) & 0xFF;
	entry->upper_base_addr = base_addr >> 32;
	entry->reserved = 0;
}

void tss_init() {
	int i;

	for (i = 0; i < CORE_COUNT; i++) {
		memset(&(mygdt.tss[i]), 0, sizeof(tss_t));
		mygdt.tss[i].ist[0] = (QWORD) &IST_CORE0 + IST_SIZE * (i + 1);
		mygdt.tss[i].io_map_base_addr = 0xFFFF; // do not use io_map now
	}
}

