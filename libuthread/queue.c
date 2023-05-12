#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "queue.h"

// structure to define the node of a queue
struct Node {
	void* data; // pointer to the data element of the node
	struct Node* next; // pointer to the next node in the queue
};

// structure to define a queue, including pointer to head of queue and pointer to end of queue, and length of queue
struct queue {
	struct Node* head;
	struct Node* end;
	int len;
};

// function to create a new queue and initialize its parameters
queue_t queue_create(void)
{
	// allocating memory for the new queue, and set its head and end to NULL
	struct queue* q = malloc(sizeof(struct queue));
	if (q == NULL)
		return NULL;
	q->head = NULL;
	q->end = NULL;
	q->len = 0;
	return q;
}

// function to destroy a queue and free its allocated memory
int queue_destroy(queue_t queue)
{
	// check if queue is not null and empty
	if (queue == NULL || queue->len != 0) {
		return -1;
	} // frees the memory allocated to the queue
	else {
		free(queue);
		return 0;
	}

}

// function to add a new node to the end of the queue
int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}
	else {
		// allocate memory for the new node, update the queue parameters and return 0
		struct Node* new = malloc(sizeof(struct Node));
		if (new == NULL) {
			return -1;
		}
		// set the node's data to data and its next pointer to nill
		new->data = data;
		new->next = NULL;
		// if there is nothing in the queue, we set the queue's head and end to new node
		if (queue->head == NULL) {
			queue->head = new;
			queue->end = new;
		}
		// else, we set the tail of the queue to point to the new node
		else {
			queue->end->next = new;
			queue->end = new;
		}
		// increment queue length
		queue->len++;
		return 0;
	}
}

// function to remove the node at the head of the queue
int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->len == 0) {
		return -1;
	}
	// set the data to be returned as the data element of the head node
	*data = queue->head->data;
	struct Node* temp = queue->head;
	// update the head of the queue to point to the next node
	queue->head = queue->head->next;

	if (queue->head == NULL)
		queue->end = NULL;

	// free the memory allocated to the previous head node
	free(temp);
	// decrement length
	queue->len--;
	return 0;
}
// function to delete a node from the queue
int queue_delete(queue_t queue, void *data)
{

	if (queue == NULL || queue->head == NULL || data == NULL) {
		return -1;
	}
	struct Node* prev = NULL;
	struct Node* curr = queue->head;
	struct Node* next = queue->head->next;

	// handle case where queue has only one element
	if (queue_length(queue) == 1) {
		if (curr->data == data) {
			free(curr);
			queue->len--;
			return 0;
		}
		else {
			return -1;
		}
	}

	// iterate through the queue
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

// function to iterate through the queue and apply a function to each node
int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL) {
		return -1;
	}
	struct Node* temp = queue->head;
	while (temp != NULL) {
		// apply the function to the node
		(*func)(queue, temp->data); 
		temp = temp->next;
	}
	return 0;
}

// function to return the length of the queue
int queue_length(queue_t queue)
{
	if (queue == NULL) {
		return -1;
	}
	return queue->len;
}