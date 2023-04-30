#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct Node {
	void* data;
	struct Node* next;
};

struct queue {
	struct Node* head;
	struct Node* end;
	int len;
};

queue_t queue_create(void)
{
	struct queue* q = malloc(sizeof(struct queue));
	if (q == NULL)
		return NULL;
	q->head = NULL;
	q->end = NULL;
	q->len = 0;
	return q;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL || queue->len != 0) {
		return -1;
	}
	else {
		free(queue);
		return 0;
	}

}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if (queue == NULL || data == NULL) {
		return -1;
	}
	else {
		struct Node* new = malloc(sizeof(struct Node));
		if (new == NULL) {
			return -1;
		}
		new->data = data;
		new->next = NULL;
		if (queue->head == NULL) {
			queue->head = new;
			queue->end = new;
		}
		else {
			queue->end->next = new;
			queue->end = new;
		}
		queue->len++;
		return 0;
	}
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
	if (queue == NULL || data == NULL || queue->len == 0) {
		return -1;
	}
	*data = queue->head->data;
	struct Node* temp = queue->head;
	queue->head = queue->head->next;
	free(temp);
	queue->len--;
	return 0;
}
//x -> x -> x
int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if (queue == NULL || queue->head == NULL || data == NULL) {
		return -1;
	}
	struct Node* prev = NULL;
	struct Node* curr = queue->head;
	struct Node* next = queue->head->next;
	while (next != NULL) {
		if (curr->data == data) {
			if (prev == NULL) { // delete head of queue
				struct Node* temp = queue->head;
				queue->head = queue->head->next;
				free(temp);
			}
			else { // delete in middle/end of queue
				prev->next = next;
				free(curr);
				queue->len--;
				return 0;
			}
		}
		prev = curr;
		curr = next;
		next = next->next;
	}
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
	if (queue == NULL || func == NULL) {
		return -1;
	}
	struct Node* temp = queue->head;
	
	while (temp != NULL) {
		func(queue, temp->data);
		temp = temp->next;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL) {
		return -1;
	}
	return queue->len;
}

