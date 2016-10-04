/*
 * dynamic.c
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */
#include "dynamic.h"
#include "sys.h"
#include "fault.h"

/*
 *  Super Simple NO-FREE HEAP
 */

static QWORD alloc_pointer = NULL;

BYTE* kmalloc(size_t size) {
	BYTE* addr;

	if (!alloc_pointer)
		alloc_pointer = &img_end;


	addr = alloc_pointer;
	alloc_pointer += size;

	return addr;
}

// get heap which is aligned by 4KB
BYTE* kmalloc_a_4kb(size_t size) {
	BYTE* addr;

	if (!alloc_pointer)
		alloc_pointer = &img_end;

	if (alloc_pointer & 0xfff) {
		alloc_pointer &= 0xfffffffffffff000;
		alloc_pointer += 0x1000;
	}

	addr = alloc_pointer;
	alloc_pointer += size;

	return addr;
}

// get heap which is aligned by 2MB
BYTE* kmalloc_a_2mb(size_t size) {
	BYTE* addr;

	if (!alloc_pointer)
		alloc_pointer = &img_end;

	if (alloc_pointer & 0x1fffff) {
		alloc_pointer &= 0xffffffffffe00000;
		alloc_pointer += 0x200000;
	}

	addr = alloc_pointer;
	alloc_pointer += size;

	return addr;
}

void kfree(BYTE* vaddr) {
	// do nothing
}
