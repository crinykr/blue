#include "fault.h"
#include "pic.h"
#include "keyboard.h"
#include "console.h"
#include "util.h"
#include "task.h"
#include "gdt.h"
#include "asm_util.h"
#include "list.h"
#include "scheduler.h"
#include "sys.h"

void device_not_avail_handler() {
	tcb_t* fpu_task, *cur_task;
	tcb_t* last_fpu_task;
	BYTE core;

	core = get_core_num();

	// clear CR0's TSbit
	clear_ts();

	// get last fpu task if it's exist
	last_fpu_task = get_last_fpu_task(core);
	cur_task = get_cur_task(core);

	// If get_cur_task is the last fpu used task, do nothing
	if (last_fpu_task == cur_task) {
		return;
	}

	// Update fpu registers
	else if (last_fpu_task) {
		fpu_task = last_fpu_task;
		if ((fpu_task != NULL) && (fpu_task == last_fpu_task)) {
			save_fpu_context(fpu_task->fpu_context);
		}
	}

	// if cur task used fpu, restore fpu registers
	// if not do nothing
	if (cur_task->fpu_used == FALSE) {
		x86_finit();
		cur_task->fpu_used = TRUE;
	} else {
		load_fpu_context(cur_task->fpu_context);
	}

	set_last_fpu_task(core, cur_task);
}

