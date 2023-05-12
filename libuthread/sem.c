#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore 
{
	// queue of blocked threads that requested a resource
	queue_t blocked_threads;
	// current count of resources held by the semaphore
	size_t count;
};

sem_t sem_create(size_t count)
{
	// instantiate the semaphore
	sem_t sem = malloc(sizeof(struct semaphore));
	if (sem == NULL)
		return sem;
	// instantiate the blocked queue
	sem->blocked_threads = queue_create();
	// set internal count
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	// check if the semaphore provided isn't null and we can destroy the blocked queue
	if (sem == NULL || queue_destroy(sem->blocked_threads) == -1) {
		return -1;
	}
	else {
		// free the semaphore if successful
		free(sem);
		return 0;
	}
}

int sem_down(sem_t sem)
{
	// check if semaphore is NULL
	if (sem == NULL) {
		return -1;
	}
	else if (sem->count == 0) {
		// if the count is 0, then we add the thread that requested the resource to the blocked queue
		// no need to decrement the count since we are already at 0
		struct uthread_tcb* requesting_thread = uthread_current();
		queue_enqueue(sem->blocked_threads, requesting_thread);
		uthread_block();
	}
	else {
		// decrement the count, letting the requested thread use the resource
		sem->count--;
	}
	return 0;
}

int sem_up(sem_t sem)
{
	// check if the semaphore is NULL
	if (sem == NULL) {
		return -1;
	}
	else {
		// check if we have any blocked threads, and release them if so
		// don't increment the count since the thread uses the new resource immediately
		if (queue_length(sem->blocked_threads) > 0) {
			struct uthread_tcb* released_thread = NULL;
			queue_dequeue(sem->blocked_threads, (void**) &released_thread);
			uthread_unblock(released_thread);
		}
		// increment the count if no threads are blocked
		else {
			sem->count++;
		}
		return 0;
	}
}

