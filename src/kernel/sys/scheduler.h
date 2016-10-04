/*
 * scheduler.h
 *
 *  Created on: 2014. 8. 7.
 *      Author: criny
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "types.h"
#include "task.h"
#include "synchronization.h"
#include "sys.h"

#pragma pack( push, 1 )
typedef struct {
	spinlock_t spinlock;

	tcb_t* cur_task[CORE_COUNT];
	tcb_t* last_fpu_task[CORE_COUNT];

	list_t ready_list;
} scheduler_t;
#pragma pack( pop )

void do_simple_schedule();

void delete_task(tcb_t *task);
void scheduler_init(void);
void set_cur_task( BYTE core, tcb_t* task);
tcb_t* get_cur_task( BYTE core);
tcb_t* pick_next_task(void);
void add_task_to_ready_list(tcb_t* task);
BOOL schedule(void);
BOOL schedule_in_intr(regs_t *r);
tcb_t* remove_task_from_ready_list(tcb_t* task);
BOOL kill_task(tcb_t* task);
void kill_me(void);
int ready_task_count(void);
tcb_t* get_proc_if_thread(tcb_t* pstThread);

tcb_t* get_last_fpu_task( BYTE core);
void set_last_fpu_task( BYTE core, tcb_t* task);

#endif /* SCHEDULER_H_ */
