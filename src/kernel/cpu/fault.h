#ifndef FAULT_H_
#define FAULT_H_

#include "types.h"
#include "sys.h"
#include "multiprocessor.h"

// ISA IRQ count is 16
// So blue handled just 16 IRQ
#define INTR_MAX_COUNT 16

void device_not_available_handler();

#endif /* FAULT_H_ */
