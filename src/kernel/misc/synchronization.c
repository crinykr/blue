#include "synchronization.h"
#include "util.h"
#include "task.h"
#include "scheduler.h"
#include "asm_util.h"

void mutex_init(mutex_t* pstmutex_t) {
	pstmutex_t->lock_flag = FALSE;
	pstmutex_t->lock_count = 0;
	pstmutex_t->task = NULL;
}

// MUTEX : To synchronize tasks
void mutex_lock(mutex_t* pstmutex_t) {
	BYTE core;
	BOOL intr_flag;

	intr_flag = set_intr( FALSE);

	core = get_core_num();

	// if already locked
	if (test_and_set(&(pstmutex_t->lock_flag), 0, 1) == FALSE) {
		// if current task locked mutex, just increase lock_count
		if (pstmutex_t->task == get_cur_task(core)) {
			set_intr(intr_flag);
			pstmutex_t->lock_count++;
			return;
		}

		// if not, wait until mutex unlocked
		while (test_and_set(&(pstmutex_t->lock_flag), 0, 1) == FALSE) {
			schedule();
		}
	}

	pstmutex_t->lock_count = 1;
	pstmutex_t->task = get_cur_task(core);
	set_intr(intr_flag);
}

void mutex_unlock(mutex_t* pstmutex_t) {
	BOOL intr_flag;

	intr_flag = set_intr( FALSE);

	// if current task not locked mutex, just return
	if ((pstmutex_t->lock_flag == FALSE)
			|| (pstmutex_t->task != get_cur_task(get_core_num()))) {
		// 인터럽트를 복원
		set_intr(intr_flag);
		return;
	}

	// if mutex locked more than one
	if (pstmutex_t->lock_count > 1) {
		pstmutex_t->lock_count--;
	}
	// if not
	else {
		pstmutex_t->task = NULL;
		pstmutex_t->lock_count = 0;
		pstmutex_t->lock_flag = FALSE;
	}
	set_intr(intr_flag);
}

// SPINLOCK : To synchronize core
void spinlock_init(spinlock_t* spinlock) {
	spinlock->lock_flag = FALSE;
	spinlock->lock_count = 0;
	spinlock->core = 0xFF;
	spinlock->intr_flag = FALSE;
}

void spinlock_lock(spinlock_t* spinlock) {
	BOOL intr_flag;

	intr_flag = set_intr(FALSE);

	// criny: 같은코어가 두번잠글일은 코딩실수 말고 없지않나?
	// if it's already locked, check I locked spinlock
	// if I locked it,
	// If spinlock already locked
	if (test_and_set(&(spinlock->lock_flag), 0, 1) == FALSE) {
		// If I locked spinlock, just increase lock_count and return
		if (spinlock->core == get_core_num()) {
			spinlock->lock_count++;
			return;
		}

		// If not, wait until spinlock unlocked
		// To prevent memory bus locked, call test_and_set continuously : criny : why??
		while (test_and_set(&(spinlock->lock_flag), 0, 1) == FALSE) {
			while (spinlock->lock_flag == TRUE) {
				pause();
			}
		}
	}

	//spinlock->lock_flag = setted in above
	spinlock->lock_count = 1;
	spinlock->core = get_core_num();

	spinlock->intr_flag = intr_flag;
}

void spinlock_unlock(spinlock_t* spinlock) {
	BOOL intr_flag;

	intr_flag = set_intr( FALSE);

	// if others locked spinklock, just return
	if ((spinlock->lock_flag == FALSE) || (spinlock->core != get_core_num())) {
		set_intr(intr_flag);
		return;
	}

	// if spinlock locked more than one, just decrease lock_count
	if (spinlock->lock_count > 1) {
		spinlock->lock_count--;
		return;
	}

	intr_flag = spinlock->intr_flag;
	spinlock->core = 0xFF;
	spinlock->lock_count = 0;
	spinlock->intr_flag = FALSE;
	spinlock->lock_flag = FALSE;

	set_intr(intr_flag);
}
