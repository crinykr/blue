#ifndef LIST_H_
#define LIST_H_

#include "types.h"

#pragma pack( push, 1 )

typedef struct list_node {
	//struct list_node *prev;
	struct list_node *next;

	void* value;
} list_node_t;

typedef struct {
	list_node_t *head;
	list_node_t *tail;

	size_t length;
} list_t;

#pragma pack( pop )

void list_init(list_t *list);
size_t list_count(const list_t *list);
void list_add_to_tail(list_t *list, list_node_t *node);
void list_add_to_head(list_t *list, list_node_t *node);
list_node_t* list_remove(list_t *list, void *value);
list_node_t* list_remove_head(list_t *list);
list_node_t* list_remove_tail(list_t *list);
list_node_t* list_find(const list_t *list, void *value);
list_node_t* list_head(const list_t *list);
list_node_t* list_tail(const list_t *list);
list_node_t* list_next(list_node_t *node);
list_node_t* list_prev(const list_t *pstlist_t, list_node_t *node);


#endif /* LIST_H_ */

