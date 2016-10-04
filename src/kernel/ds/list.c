#include "list.h"

void list_init(list_t* list) {
	list->length = 0;
	list->head = NULL;
	list->tail = NULL;
}

size_t list_count(const list_t* list) {
	return list->length;
}

void list_add_to_tail(list_t* list, list_node_t* node) {
	node->next = NULL;

	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
		list->length = 1;
		return;
	}

	list->tail->next = node;
	list->tail = node;
	list->length++;
}

void list_add_to_head(list_t* list, list_node_t* node) {
	node->next = list->head;

	if (list->head == NULL) {
		list->head = node;
		list->tail = node;
		list->length = 1;
		return;
	}

	list->head = node;
	list->length++;
}

// remove the node and return removed node address
list_node_t* list_remove(list_t* list, void* value) {
	list_node_t *cur, *previous;

	for (cur = list->head; cur != NULL; cur = cur->next) {

		if (cur->value == value) {
			// 1. if list have only one node, initilize list
			if ((cur == list->head) && (cur == list->tail)) {
				list->head = NULL;
				list->tail = NULL;
			}

			// 2. if head of list, just change head to next
			else if (cur == list->head) {
				list->head = cur->next;
			}

			// 3. if tail of list, change tail to previous
			else if (cur == list->tail) {
				list->tail = previous;
				list->tail->next = NULL;
			}

			// 4. etc
			else {
				previous->next = cur->next;
			}

			cur->next = NULL;
			list->length = list->length - 1;
			return cur;
		}
		previous = cur;
	}
	return NULL;
}

list_node_t* list_remove_head(list_t* list) {
	void* a;
	if (list->length == 0) {
		return NULL;
	}


	//criny(list); // ffff800000124ed8
	//criny(list->head);// 0000000000125fec

	return list_remove(list, list->head->value);
}

list_node_t* list_remove_tail(list_t* list) {
	if (list->length == 0) {
		return NULL;
	}

	return list_remove(list, list->tail->value);
}

list_node_t* list_find(const list_t* list, void* value) {
	list_node_t* cur;

	for (cur = (list_node_t*) list->head; cur != NULL; cur = cur->next) {
		if (cur->value == value) {
			return cur;
		}
	}
	return NULL;
}

list_node_t* list_head(const list_t* list) {
	return list->head;
}

list_node_t* list_tail(const list_t* list) {
	return list->tail;
}

list_node_t* list_next(list_node_t* node) {
	return node->next;
}

list_node_t* list_prev(const list_t* list, list_node_t* node) {
	list_node_t* tmp;

	for (tmp = (list_node_t*) list->head; tmp != NULL; tmp = tmp->next) {
		if (tmp->next == node) {
			return tmp;
		}
	}
	return NULL;
}
