/*
 * dynamic.h
 *
 *  Created on: 2014. 8. 21.
 *      Author: criny
 */

#ifndef DYNAMIC_H_
#define DYNAMIC_H_

#include "types.h"

BYTE* kmalloc(size_t size);
BYTE* kmalloc_a_4kb(size_t size);
BYTE* kmalloc_a_2mb(size_t size);

void kfree(BYTE* vaddr);

#endif /* DYNAMIC_H_ */
