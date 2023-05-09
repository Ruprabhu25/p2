#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

 #define RUNNING 0
 #define READY 1
 #define BLOCKED 2
 #define DONE 3

int cur_id = 1;
queue_t queue;
struct uthread_tcb* current_thread;
struct uthread_tcb* idle_thread;


struct uthread_tcb {
	/* TODO Phase 2 */
	int id;
	int state; /* 0 indicates ready to run, 1 indicates running, 2 indicate exit*/
	void* stack_ptr;
	uthread_ctx_t* context;
};

struct uthread_tcb *uthread_current(void)
{
	return current_thread;
}

// get next ready node
/*void get_next(queue_t queue, void* node) {
	struct uthread_tcb* thread = node;
	if (current_thread->id == thread->id) {
		return; // ignore current thread
	}
	if (thread->state == READY) {
		uthread_ctx_switch(current_thread->context, thread->context);
		current_thread->state = READY;
		current_thread = thread;
		current_thread->state = RUNNING;
		return;
		// need to add some sort of flag indicating we do not need to iterate further
	}

}*/

void uthread_yield(void) {
	struct uthread_tcb* previous_thread = uthread_current();
	struct uthread_tcb* next_thread = NULL;
	previous_thread->state = READY;
	queue_dequeue(queue, (void**) &next_thread);
	current_thread = next_thread;
	current_thread->state = RUNNING;
	if (idle_thread == next_thread && queue_length(queue) > 1) {
		next_thread->state = READY;
		queue_dequeue(queue, (void**) &next_thread);
		next_thread->state = RUNNING;
		current_thread = next_thread; 
		queue_enqueue(queue, (void**) &next_thread);
	}

	queue_enqueue(queue, (void**) &previous_thread);
	uthread_ctx_switch(previous_thread->context, next_thread->context);
}

void uthread_exit(void) {
	/* TODO Phase 2 */
	current_thread->state = DONE;
	struct uthread_tcb* done_thread = uthread_current();
	uthread_yield();
	queue_delete(queue, done_thread);
	uthread_ctx_destroy_stack(done_thread->stack_ptr);
	free(done_thread);
}

int uthread_create(uthread_func_t func, void *arg) {
	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));
	if (thread == NULL) {
		return -1;
	}
	thread->stack_ptr = uthread_ctx_alloc_stack();
	thread->state = RUNNING;
	thread->context = malloc(sizeof(uthread_ctx_t));
	if (thread->stack_ptr == NULL || thread->context == NULL) {
		return -1;
	}
	if (uthread_ctx_init(thread->context, thread->stack_ptr,func, arg) == -1)
		return -1;
	//printf("here\n");
	//func(arg);
	queue_enqueue(queue, (void**) &thread);
	return 0;
	/* TODO Phase 2 */
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	//printf("run\n");
	queue = queue_create();
	idle_thread = malloc(sizeof(struct uthread_tcb));
	if (idle_thread == NULL) {
		return -1;
	}
	idle_thread->context = malloc(sizeof(uthread_ctx_t));
	if (idle_thread->context == NULL) {
		return -1;
	}
	current_thread = idle_thread;
	current_thread->state = RUNNING;
	//printf("created queue\n");
	if (uthread_create(func, arg) == -1) {
		//printf("failed\n");
		return -1;
	}

	while (queue_length(queue) != 0) {
		uthread_yield();
	}

	free(idle_thread->context);
	uthread_ctx_destroy_stack(idle_thread->stack_ptr);
	free(idle_thread);
	//printf("length: %d\n", queue_length(queue));
	return 0;
}

void uthread_block(void)
{
	current_thread->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
	queue_enqueue(queue, (void**) &uthread);
}

