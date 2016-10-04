/*
 * page.h
 *
 *  Created on: 2014. 8. 17.
 *      Author: criny
 */

#ifndef PAGE_H_
#define PAGE_H_

#include "types.h"

#define GET_OFFSET(addr,a,b) (addr << (63 - a) >> (63 - a + b))
#define OFFSET_PML4(vaddr) GET_OFFSET(vaddr,47,39)
#define OFFSET_PDPT(vaddr) GET_OFFSET(vaddr,38,30)
#define OFFSET_PD(vaddr) GET_OFFSET(vaddr,29,21)
#define OFFSET_2MB_FRAME(vaddr) GET_OFFSET(vaddr,20,0)

/* *****************************************
 * x86 page structure naming rules in blue
 * *****************************************
 * PML4 table = pml4
 * page-directory-pointer table = pdpt
 * page directory = pd
 * page table = pt
 */

/*
 * Caution:
 * reserved field must be 0
 */
typedef struct {
	unsigned int present :1;
	unsigned int rw :1;
	unsigned int user :1; // 1:user, 0:kernel
	unsigned int pwt :1;
	unsigned int pcd :1;
	unsigned int accessed :1;
	unsigned int res1 :3;
	unsigned int avail1 :3;
	unsigned int addr :28;
	unsigned int res2 :12;
	unsigned int avail2 :11;
	unsigned int exb :1;
}__attribute__((packed)) pml4e_t, pdpte_t;

typedef struct {
	unsigned int present :1;
	unsigned int rw :1;
	unsigned int user :1;
	unsigned int pwt :1;
	unsigned int pcd :1;
	unsigned int accessed :1;
	unsigned int dirty :1;
	unsigned int ps :1;
	unsigned int g :1;
	unsigned int avail1 :3;
	unsigned int pat :1;
	unsigned int res1 :8;
	unsigned int addr :19;
	unsigned int res :12;
	unsigned int avail2 :11;
	unsigned int exb :1;
}__attribute__((packed)) pde_t;

#if 0
typedef struct {
	unsigned int present:1;
	unsigned int rw:1;
	unsigned int user:1;
	unsigned int pwt:1;
	unsigned int pcd:1;
	unsigned int accessed:1;
	unsigned int dirty:1;
	unsigned int pat:1;
	unsigned int g:1;
	unsigned int avail1:3;
	unsigned int addr:28;
	unsigned int res:12;
	unsigned int avail2:11;
	unsigned int exb:1;
}__attribute__((packed)) pte_t;
#endif

typedef struct {
	pml4e_t entry[512];
} pml4_t;

typedef struct {
	pdpte_t entry[512];
} pdpt_t;

typedef struct {
	pde_t entry[512];
} pd_t;

#if 0
typedef struct {
	pte_t entry[512];
}pt_t;
#endif

void paging_init(QWORD memsize_kb);
pde_t *get_page(QWORD address, int make, pml4_t *dir);
void alloc_frame(pde_t *page, int is_kernel, int is_writeable);

void switch_pml4(pml4_t *pml4_vaddr);
pml4_t * clone_pml4(pml4_t * src);
pdpt_t* clone_pdpt(pdpt_t * src);
pd_t* clone_pd(pd_t * src);
void copy_2mb_page(unsigned long *dst, unsigned long *src);

//DWORD first_frame(void);
#endif /* PAGE_H_ */
