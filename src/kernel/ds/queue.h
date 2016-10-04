#ifndef QUEUE_H_
#define QUEUE_H_

#include "types.h"

#define QUEUE_DEQUEUE 0 // remove from queue
#define QUEUE_INQUEUE 1 // insert to queue

#pragma pack( push, 1 )
typedef struct queue_struct {
	int item_size;
	int max_item_count;

	void* array_addr;
	int put_index;
	int get_index;

	BOOL last_op;
} queue_t;
#pragma pack( pop )

void queue_init(queue_t* queue, void* buffer, int max_item_count,
		int item_size);
BOOL queue_is_full(const queue_t* queue);
BOOL queue_is_empty(const queue_t* queue);
BOOL inqueue(queue_t* queue, const void* item);
BOOL dequeue(queue_t* queue, void* item);

#endif /* QUEUE_H_ */
