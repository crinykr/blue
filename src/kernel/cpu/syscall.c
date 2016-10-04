#include "syscall.h"
#include "task.h"
#include "scheduler.h"

void bash_main() {

	while (1);
		//printf("bash");
}

void execve() {
	// 1. COPY user program FROM local disk TO memory
	// 2. MAPPING page table
	// 3, RUN

}

void syscall_handler() {
	int syscall_num = get_cur_task(get_core_num())->context.rax;

	switch (syscall_num) {
	case 0:
		fork();
		return;
	case 1:
		do_simple_schedule();
		return;
	case 2:
		execve();
		return;
	}
}

