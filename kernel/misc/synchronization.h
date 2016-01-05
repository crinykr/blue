#ifndef SYNCHRONIZATION_H_
#define SYNCHRONIZATION_H_

#include "types.h"
#include "task.h"

#pragma pack( push, 1 )
typedef struct {
	volatile tcb_t* task;
	volatile DWORD lock_count;
	volatile BOOL lock_flag;

	BYTE padding[3];
} mutex_t;

typedef struct {
	volatile DWORD lock_count;
	volatile BYTE core;
	volatile BOOL lock_flag;
	volatile BOOL intr_flag;

	BYTE padding[1];
} spinlock_t;

#pragma pack( pop )

void mutex_init(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);
void mutex_unlock(mutex_t* mutex);

void spinlock_init(spinlock_t* spinlock);
void spinlock_lock(spinlock_t* spinlock);
void spinlock_unlock(spinlock_t* spinlock);

#endif /* SYNCHRONIZATION_H_ */
