#include "queue.h"

void queue_init(queue_t* queue, void* buffer, int max_item_count, int item_size) {
	queue->max_item_count = max_item_count;
	queue->item_size = item_size;
	queue->array_addr = buffer;

	queue->put_index = 0;
	queue->get_index = 0;
	queue->last_op = QUEUE_DEQUEUE;
}

BOOL queue_is_full(const queue_t* queue) {
	if ((queue->get_index == queue->put_index)
			&& (queue->last_op == QUEUE_INQUEUE))
		return TRUE;

	return FALSE;
}

BOOL queue_is_empty(const queue_t* queue) {
	if ((queue->get_index == queue->put_index)
			&& (queue->last_op == QUEUE_DEQUEUE))
		return TRUE;

	return FALSE;
}

BOOL inqueue(queue_t* queue, const void* item) {
	if (queue_is_full(queue) == TRUE)
		return FALSE;

	memcpy((char*) queue->array_addr + (queue->item_size * queue->put_index),
			item, queue->item_size);

	queue->put_index = (queue->put_index + 1) % queue->max_item_count;
	queue->last_op = QUEUE_INQUEUE;
	return TRUE;
}

BOOL dequeue(queue_t* queue, void* item) {
	if (queue_is_empty(queue) == TRUE)
		return FALSE;

	memcpy(item,
			(char*) queue->array_addr + (queue->item_size * queue->get_index),
			queue->item_size);

	queue->get_index = (queue->get_index + 1) % queue->max_item_count;
	queue->last_op = QUEUE_DEQUEUE;
	return TRUE;
}
