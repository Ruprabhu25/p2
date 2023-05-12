#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> //delete
#include "queue.h"
#include "sem.h"
#include "private.h"
int sid = 1;
struct semaphore {
	int id;
	queue_t blocked_threads;
	size_t count;
};

sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(struct semaphore));
	if (sem == NULL)
		return sem;
	sem->id = sid;
	sid++;
	sem->blocked_threads = queue_create();
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_destroy(sem->blocked_threads) == -1) {
		return -1;
	}
	else {
		free(sem);
		return 0;
	}
}

int sem_down(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	else if (sem->count == 0) {
		struct uthread_tcb* requesting_thread = uthread_current();
		queue_enqueue(sem->blocked_threads, requesting_thread);
		uthread_block();
	}
	else {
		sem->count--;
	}
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL) {
		return -1;
	}
	else {
		if (queue_length(sem->blocked_threads) > 0) {
			struct uthread_tcb* released_thread = NULL;
			queue_dequeue(sem->blocked_threads, (void**) &released_thread);
			uthread_unblock(released_thread);
		}
		else {
			sem->count++;
			}
		return 0;
	}
}

