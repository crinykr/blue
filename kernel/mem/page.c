/*
 * page.c
 *
 *  Created on: 2014. 8. 17.
 *      Author: criny
 */

#include "page.h"
#include "sys.h"
#include "util.h"
#include "asm_util.h"
#include "dynamic.h"

//static BYTE *frames; // bitmap data to handle used page
//static DWORD nframes;

#define KERNEL_START 0xffffffff80000000

pml4_t *kernel_pml4;

// make page management bitmap structure
void paging_init(QWORD memsize_kb) {
	QWORD i, j;

//	nframes = memsize / 4;
//	frames = kmalloc((nframes + 7) / 8);
//	memset(frames, 0, (nframes + 7) / 8);

// make genuine kernel page tables by using calculating memory size

// trim early mapping
	*(QWORD*) (&EARLY_PGT) = NULL;

	kernel_pml4 = &EARLY_PGT;
}

// Get PAGE DIRECTORY ENTRY virtual address
// : just allocate space for page directory not page entry
//   page entry is allocated in "alloc_frame"

// Post Condition :
// - page table physical address must not be "0"
// - value 0 say its entry is empty so not indicate sub table start address
pde_t* get_page(QWORD vaddr, int make, pml4_t *pml4) {
	QWORD a, b;
	vaddr &= 0xffffffffffff;

	// if has PDPT
	if (pml4->entry[OFFSET_PML4(vaddr)].addr) {
		a = pml4->entry[OFFSET_PML4(vaddr)].addr;
		a <<= 12;
		a += PAGE_OFFSET;

		// if has PD
		if (((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].addr) {
			b = ((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].addr;
			b <<= 12;
			b += PAGE_OFFSET;

			return &((pd_t*) b)->entry[OFFSET_PD(vaddr)];
		}
		// if not has PD, make PD
		else if (make) {
			b = kmalloc_a_4kb(sizeof(pd_t));
			memset(b, 0, sizeof(pd_t));
			b -= PAGE_OFFSET;
			b >>= 12;

			((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].addr = b;
			((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].present = 1;
			((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].rw = 1;
			((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].user = 1;

			b <<= 12;
			b += PAGE_OFFSET;

			return &((pd_t*) b)->entry[OFFSET_PD(vaddr)];
		} else {
			return 0;
		}
	}
	// if not has PDPT, make PDPT and PD
	else if (make) {
		// make PDPT
		a = kmalloc_a_4kb(sizeof(pdpt_t));
		memset(a, 0, sizeof(pdpt_t));
		a -= PAGE_OFFSET;
		a >>= 12;

		pml4->entry[OFFSET_PML4(vaddr)].addr = a;
		pml4->entry[OFFSET_PML4(vaddr)].present = 1;
		pml4->entry[OFFSET_PML4(vaddr)].rw = 1;
		pml4->entry[OFFSET_PML4(vaddr)].user = 1;

		a <<= 12;
		a += PAGE_OFFSET;

		// make PD
		b = kmalloc_a_4kb(sizeof(pd_t));
		memset(b, 0, sizeof(pd_t));
		b -= PAGE_OFFSET;
		b >>= 12;

		((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].addr = b;
		((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].present = 1;
		((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].rw = 1;
		((pdpt_t*) a)->entry[OFFSET_PDPT(vaddr)].user = 1;

		b <<= 12;
		b += PAGE_OFFSET;

		return &((pd_t*) b)->entry[OFFSET_PD(vaddr)];

	} else {
		return 0;
	}
}

void alloc_frame(pde_t *page, int is_kernel, int is_writeable) {
	QWORD vaddr, index;

	vaddr = kmalloc_a_2mb(0x200000);
	index = (vaddr - PAGE_OFFSET) >> 21;

	page->present = 1;
	page->rw = (is_writeable == 1) ? 1 : 0;
	page->user = (is_kernel == 1) ? 0 : 1;
	page->addr = index;
}

void switch_pml4(pml4_t *pml4_vaddr) {
	load_cr3((QWORD) pml4_vaddr - PAGE_OFFSET);
}

/*
 * Clone a pml4 and its contents.
 */
pml4_t* clone_pml4(pml4_t * src) {
	int i;

	/* Allocate a new page directory */
	pml4_t *pml4 = (pml4_t*) kmalloc_a_4kb(sizeof(pml4_t));
	memset(pml4, 0, sizeof(pml4_t));

	/* Copy each pdpt */
	for (i = 0; i < 512; ++i) {
		/* if empty entry, do nothing */
		if (*(QWORD*) &(src->entry[i]) == NULL) {
			;
		}
		/* Kernel pdpt are simply linked together */
		else if (*(QWORD*) &(src->entry[i])
				== *(QWORD*) &(kernel_pml4->entry[i])) {
			pml4->entry[i] = src->entry[i];
		}
		/* User pdpt must be cloned */
		else {
			pml4->entry[i].addr = (QWORD) clone_pdpt(src->entry[i].addr >> 12)
					<< 12;
			pml4->entry[i].present = 1;
			//...

			//dir->physical_tables[i] = phys | 0x07;
			//if (i * 512 * 512 * 0x200000 < KERNEL_START) { // this is dummy maybe
			//;
			//}
		}
	}
	return pml4;
}

/*
 * Clone a page directory pointer table
 *
 * @param src      Pointer to a page directory pointer table to clone.
 * @return         A pointer to a new page table.
 */
pdpt_t* clone_pdpt(pdpt_t * src) {
	int i;

	/* Allocate a new page pdpt */
	pdpt_t * pdpt = (pdpt_t*) kmalloc_a_4kb(sizeof(pdpt_t));
	memset(pdpt, 0, sizeof(pdpt_t));

	/* Copy pdpt */
	for (i = 0; i < 512; ++i) {
		// if empty pdpt entry
		if (*(QWORD*) &(src->entry[i]) == NULL) {
			;
		}
		// if not empty
		else {
			pdpt->entry[i].addr = (QWORD) clone_pd(src->entry[i].addr >> 12)
					<< 12;
			pdpt->entry[i].present = 1;
		}
	}
	return pdpt;
}

/*
 * Clone a page directory
 *
 * @param src      Pointer to a page directory to clone.
 * @return         A pointer to a new page table.
 */
pd_t* clone_pd(pd_t * src) {
	int i;

	/* Allocate a new page table */
	pd_t * pd = (pd_t *) kmalloc_a_4kb(sizeof(pd_t));
	memset(pd, 0, sizeof(pd_t));

	for (i = 0; i < 512; ++i) {
		if (*(QWORD*)&(src->entry[i]) == NULL) {
			continue;
		}

		/* Allocate a new frame */
		alloc_frame(&pd->entry[i], 0, 0);

		/* Set the correct access bit */
		if (src->entry[i].present)
			pd->entry[i].present = 1;
		if (src->entry[i].rw)
			pd->entry[i].rw = 1;
		if (src->entry[i].user)
			pd->entry[i].user = 1;
		if (src->entry[i].accessed)
			pd->entry[i].accessed = 1;
		if (src->entry[i].dirty)
			pd->entry[i].dirty = 1;

		/* Copy the contents of the page from the old table to the new one */
		copy_2mb_page(pd->entry[i].addr * 0x200000,
				src->entry[i].addr * 0x200000);
	}
	return pd;
}

void copy_2mb_page(unsigned long *dst, unsigned long *src) {
	int i;

	for (i = 0; i < (0x200000 / 8); i++)
		dst[i] = src[i];
}

//DWORD first_frame(void) {
//	DWORD i, j;
//
//	for (i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
//		if (frames[i] != 0xFFFFFFFF) {
//			for (j = 0; j < 32; ++j) {
//				uint32_t testFrame = 0x1 << j;
//				if (!(frames[i] & testFrame)) {
//					return i * 0x20 + j;
//				}
//			}
//		}
//	}
//
//	kprintf(
//			"\033[1;37;41mWARNING: System claims to be out of usable memory, which means we probably overwrote the page frames.\033[0m\n");
//
//#if 0
//	signal_t * sig = kmalloc(sizeof(signal_t));
//	sig->handler = current_process->signals.functions[SIGSEGV];
//	sig->signum = SIGSEGV;
//	handle_signal((process_t *)current_process, sig);
//#endif
//
//	STOP;
//
//	return -1;
//}

