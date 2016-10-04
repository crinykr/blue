/*
 * scheduler.c
 *
 *  Created on: 2014. 8. 7.
 *      Author: criny
 */

#include "scheduler.h"
#include "multiprocessor.h"
#include "gdt.h"
#include "sys.h"
#include "list.h"
#include "task.h"
#include "sys.h"
#include "util.h"
#include "dynamic.h"
#include "synchronization.h"
#include "page.h"

scheduler_t scheduler;

void do_simple_schedule() {
	tcb_t *cur_task, *next_task;
	DWORD core;

	core = get_core_num();

	spinlock_lock(&(scheduler.spinlock));

	next_task = pick_next_task();

	if (!next_task) {
		spinlock_unlock(&(scheduler.spinlock));
		return;
	}

	cur_task = scheduler.cur_task[core];
	scheduler.cur_task[core] = next_task;

	if (scheduler.last_fpu_task[core] != next_task) {
		set_ts();
	} else {
		clear_ts();
	}

	// why don't change CR3 Register(PML4 Table Addr)?????
	add_task_to_ready_list(cur_task);
	spinlock_unlock(&(scheduler.spinlock));
	// not here after switch_context
}

// Caution : Do not call in interrupt routines
// except when killing current task
BOOL schedule(void) {
	tcb_t *cur_task, *next_task;
	BOOL pre_intr;
	BYTE core;

	pre_intr = set_intr(FALSE);
	core = get_core_num();

	if (ready_task_count() < 1) {
		set_intr(pre_intr);
		return FALSE;
	}

	spinlock_lock(&(scheduler.spinlock));

	next_task = pick_next_task();

	if (!next_task) {
		spinlock_unlock(&(scheduler.spinlock));
		set_intr(pre_intr);
		return FALSE;
	}

	cur_task = scheduler.cur_task[core];
	scheduler.cur_task[core] = next_task;

	if (scheduler.last_fpu_task[core] != next_task) {
		set_ts();
	} else {
		clear_ts();
	}

	// why don't change CR3 Register(PML4 Table Addr)?????

	// [1] just in initial AP schedule()
	if (!cur_task) {
		spinlock_unlock(&(scheduler.spinlock));
		switch_context(NULL, &(next_task->context));
	}
	// [2] if current task is die
	else if (cur_task->flags & TASK_FLAGS_ENDTASK) {
		delete_task(cur_task);
		spinlock_unlock(&(scheduler.spinlock));
		switch_context(NULL, &(next_task->context));
	}
	// [3]
	else {
		add_task_to_ready_list(cur_task);
		spinlock_unlock(&(scheduler.spinlock));
		printk("www: %s[%d]", next_task->name, next_task->id);
		cmd_ps();
		switch_context(&(cur_task->context), &(next_task->context));
	}
	// not here after switch_context

	set_intr(pre_intr);
	return TRUE;
}

// Caution : Must called in interrupt routines
BOOL schedule_in_intr(regs_t *r) {
	tcb_t* cur_task, *next_task;
	BYTE core;

	core = get_core_num();

	spinlock_lock(&(scheduler.spinlock));

	next_task = pick_next_task();
	if (!next_task) {
		spinlock_unlock(&(scheduler.spinlock));
		return FALSE;
	}

	cur_task = scheduler.cur_task[core];
	scheduler.cur_task[core] = next_task;

	// [1] just in initial AP schedule()
	if (!cur_task) {
	}
	// [2] if current task is die
	else if (cur_task->flags & TASK_FLAGS_ENDTASK) {
		delete_task(cur_task);
	}
	// [3]
	else {
		add_task_to_ready_list(cur_task); // criny
		memcpy(&(cur_task->context), r, sizeof(regs_t));
	}

	if (scheduler.last_fpu_task[core] != next_task) {
		set_ts();
	} else {
		clear_ts();
	}

	spinlock_unlock(&(scheduler.spinlock));

	memcpy(r, &(next_task->context), sizeof(regs_t));

	return TRUE;
}

void delete_task(tcb_t *task) {
	// remove task memory resource
}

void scheduler_init(void) {
	int i;

	list_init(&(scheduler.ready_list));
	spinlock_init(&(scheduler.spinlock));

	for (i = 0; i < CORE_COUNT; i++) {
		scheduler.cur_task[i] = NULL;
		scheduler.last_fpu_task[i] = NULL;
	}
	printk("scheduler [ok]\n");
}

void set_cur_task(BYTE core, tcb_t* task) {
	spinlock_lock(&(scheduler.spinlock));
	scheduler.cur_task[core] = task;
	spinlock_unlock(&(scheduler.spinlock));
}

tcb_t* get_cur_task(BYTE core) {
	tcb_t* cur_task;

	spinlock_lock(&(scheduler.spinlock));
	cur_task = scheduler.cur_task[core];
	spinlock_unlock(&(scheduler.spinlock));

	return cur_task;
}

tcb_t* pick_next_task(void) {
	list_node_t *node;

	node = list_remove_head(&(scheduler.ready_list));

	if (node)
		return node->value;
	return NULL;
}

// Caution : it's task add in same core
void add_task_to_ready_list(tcb_t *task) {
	spinlock_lock(&(scheduler.spinlock));
	list_add_to_tail(&(scheduler.ready_list), &(task->sched_link));
	spinlock_unlock(&(scheduler.spinlock));
}

extern volatile DWORD jiffies;

tcb_t* remove_task_from_ready_list(tcb_t* task) {
	return list_remove(&(scheduler.ready_list), task);
}

BOOL kill_task(tcb_t* task) {
	BYTE core;
	tcb_t *victim;

	spinlock_lock(&(scheduler.spinlock));

	victim = remove_task_from_ready_list(task);

	// if task is just in ready list
	if (victim) {
		remove_task_from_ready_list(task);
		delete_task(task);
	}
	// if not, then task is on core
	else {
		task->flags |= TASK_FLAGS_ENDTASK; // task will die in schedule function

		// if task i`s on current core
		if (get_core_num() == core) {
			schedule();
			_BARRIER_;
		}
	}
	spinlock_unlock(&(scheduler.spinlock));

	return TRUE;
}

void kill_me(void) {
	kill_task(scheduler.cur_task[get_core_num()]);
}

int ready_task_count(void) {
	int total;

	spinlock_lock(&(scheduler.spinlock));

	total = list_count(&(scheduler.ready_list));

	spinlock_unlock(&(scheduler.spinlock));
	return total;
}

tcb_t* get_proc_if_thread(tcb_t* task) {
	// if task is process, just return itself
	if (task->flags & TASK_FLAGS_PROCESS) {
		return task;
	}

	return task->parent_process;
}

tcb_t* get_last_fpu_task(BYTE core) {
	return scheduler.last_fpu_task[core];
}

void set_last_fpu_task(BYTE core, tcb_t* task) {
	scheduler.last_fpu_task[core] = task;
}

