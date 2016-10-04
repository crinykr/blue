#ifndef ASSEMBLYUTILITY_H_
#define ASSEMBLYUTILITY_H_

#include "types.h"
#include "task.h"
#include "sys.h"

void load_cr3(DWORD addr);

void lock_inc_dword(DWORD* addr);
void load_gdtr( QWORD addr);
void load_tr( WORD offset);
void load_idtr( QWORD addr);
void enable_intr(void);
void disable_intr(void);
QWORD read_rflags(void);
void switch_context(regs_t* cur_context, regs_t* next_context);
void hlt(void);
BOOL test_and_set(volatile BYTE* dest, BYTE compare, BYTE source);
void x86_finit(void);
void save_fpu_context(void* addr);
void load_fpu_context(void* addr);
void set_ts(void);
void clear_ts(void);
void enable_global_lapic(void);
void pause(void);
void write_msr( QWORD addr, QWORD upper32, QWORD lower32);

#endif /* ASSEMBLYUTILITY_H_ */
