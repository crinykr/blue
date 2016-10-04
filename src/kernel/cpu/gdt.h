#ifndef GDT_H__
#define GDT_H__

#include "types.h"

// x86 GDT ATTR
#define GDT_TYPE_CODE           0x0A
#define GDT_TYPE_DATA           0x02
#define GDT_TYPE_TSS            0x09
#define GDT_FLAGS_LOWER_S       0x10
#define GDT_FLAGS_LOWER_DPL0    0x00
#define GDT_FLAGS_LOWER_DPL1    0x20
#define GDT_FLAGS_LOWER_DPL2    0x40
#define GDT_FLAGS_LOWER_DPL3    0x60
#define GDT_FLAGS_LOWER_P       0x80 // present#define GDT_FLAGS_UPPER_L       0x20 // 64bit mode#define GDT_FLAGS_UPPER_DB      0x40#define GDT_FLAGS_UPPER_G       0x80 // granulaty#define SELECTOR_RPL_0          0x00#define SELECTOR_RPL_3          0x03// BLUE GDT FLAGS#define GDT_FLAGS_LOWER_KERNELCODE \
	( GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )#define GDT_FLAGS_LOWER_KERNELDATA \
	( GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )#define GDT_FLAGS_LOWER_USERCODE \
	( GDT_TYPE_CODE | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )#define GDT_FLAGS_LOWER_USERDATA \
	( GDT_TYPE_DATA | GDT_FLAGS_LOWER_S | GDT_FLAGS_LOWER_DPL3 | GDT_FLAGS_LOWER_P )#define GDT_FLAGS_LOWER_TSS \
	( GDT_FLAGS_LOWER_DPL0 | GDT_FLAGS_LOWER_P )

#define GDT_FLAGS_UPPER_CODE ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_DATA ( GDT_FLAGS_UPPER_G | GDT_FLAGS_UPPER_L )
#define GDT_FLAGS_UPPER_TSS  ( GDT_FLAGS_UPPER_G )

// BLUE GDT OFFSET
#define GDT_KERNELCODESEGMENT   0x08
#define GDT_KERNELDATASEGMENT   0x10
#define GDT_USERDATASEGMENT     0x18
#define GDT_USERCODESEGMENT     0x20
#define GDT_TSSSEGMENT          0x28

// BLUE IST size
#define IST_SIZE                8096

#pragma pack( push, 1 )
typedef struct {
	WORD limit;
	QWORD base_addr;
	BYTE padded[6]; // to align by 16 byte
} gdtr_t, idtr_t;

typedef struct {
	WORD lower_limit;
	WORD lower_base_addr;
	BYTE upper_base_addr1;
	BYTE type_and_lower_flag; // Type:4, S:1, DPL:2, P:1
	BYTE upper_limit_and_upper_flag; // Segment_Limit:4, AVL:1, L, D/B, G
	BYTE upper_base_addr2;
} gdt_entry8_t;

typedef struct {
	WORD lower_limit;
	WORD lower_base_addr;
	BYTE middle_base_addr1;
	BYTE type_and_lower_flag; // Type:4, null:1, DPL:2, P:1
	BYTE upper_limit_and_upper_flag; // Segment_Limit:4, AVL:1, 0, 0, G
	BYTE middle_base_addr2;
	DWORD upper_base_addr;
	DWORD reserved;
} gdt_entry16_t;

typedef struct {
	DWORD reserved0;
	QWORD rsp[3];
	QWORD reserved1;
	QWORD ist[7]; // ist[0] is only used in blue
	QWORD reserved2;
	WORD reserved3;
	WORD io_map_base_addr;
} tss_t;

struct {
	gdtr_t gdtr;
	struct {
		gdt_entry8_t null;
		gdt_entry8_t kernel_code;
		gdt_entry8_t kernel_data;
		gdt_entry8_t user_data;
		gdt_entry8_t user_code;
		gdt_entry16_t tss[4];
	} entries;
	tss_t tss[4];
} mygdt;
#pragma pack ( pop )

void gdt_tss_init(void);
void set_gdt_entry8(gdt_entry8_t* entry, DWORD base_addr, DWORD limit,
		BYTE upper_flags, BYTE lower_flags, BYTE type);
void set_gdt_entry16(gdt_entry16_t* entry, QWORD base_addr, DWORD limit,
		BYTE upper_flags, BYTE lower_flags, BYTE type);
void tss_init();

#endif /* GDT_H_ */
