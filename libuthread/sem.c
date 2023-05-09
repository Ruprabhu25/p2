#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
	queue_t blocked_threads;
	int count;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = malloc(sizeof(struct semaphore));
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
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
	/* TODO Phase 3 */
	if (sem == NULL) {
		return -1;
	}
	else if (sem->count == 0) {
		struct uthread_tcb* requesting_thread = uthread_current();
		queue_enqueue(sem->blocked_threads, (void**) &requesting_thread);
		uthread_block();
		return 0;
	}
	else {
		sem->count--;
		return 0;
	}
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
	if (sem == NULL) {
		return -1;
	}
	else {
		if (queue_length(sem->blocked_threads) > 0) {
			struct uthread_tcb* released_thread = NULL;
			queue_dequeue(sem->blocked_threads, (void**) &released_thread);
			uthread_unblock(released_thread);
		}

		sem->count++;
		return 0;
	}
}

