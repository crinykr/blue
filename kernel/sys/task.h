#ifndef TASK_H_
#define TASK_H_

#include "types.h"
#include "list.h"
#include "fault.h"
#include "sys.h"
#include "page.h"

#define KERNEL_STACK_SIZE           8096

// flags
#define TASK_FLAGS_ENDTASK          0x8000000000000000
#define TASK_FLAGS_SYSTEM           0x4000000000000000
#define TASK_FLAGS_PROCESS          0x2000000000000000
#define TASK_FLAGS_THREAD           0x1000000000000000
#define TASK_FLAGS_IDLE             0x0800000000000000
#define TASK_FLAGS_USERLEVEL        0x0400000000000000

typedef struct image {
	QWORD kernel_stack; /* Process kernel stack */
} image_t;

typedef struct tcb_struct {
	image_t image;
	QWORD id;
	struct tcb_struct* parent_process;
	char *name;

	regs_t context;
	BOOL fpu_used;
// fpu data must aligned by 16-bytes - criny: why??
	QWORD fpu_context[512 / 8];

	pml4_t *pml4;

// for scheduler
	list_node_t sched_link;

	void* mem_addr; // dummy right now, below 4 things are substituted by context.xxx
	QWORD mem_size; // dummy right now

	QWORD flags;

	BYTE core; // 0xFF mean its virgin now
} tcb_t;

void be_a_init(void);
int fork();

#endif /* TASK_H_ */
