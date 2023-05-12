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

int tid = 40;
queue_t queue;
static struct uthread_tcb* current_thread;
static struct uthread_tcb* idle_thread;


struct uthread_tcb {
	int id;
	int state; /* 0 indicates ready to run, 1 indicates running, 2 indicate exit*/
	void* stack_ptr;
	uthread_ctx_t* context;
};

struct uthread_tcb *uthread_current(void)
{
	return current_thread;
}

void uthread_yield(void) {
	
	struct uthread_tcb* previous_thread = uthread_current();
	struct uthread_tcb* next_thread = NULL;
	//previous_thread->state = READY;
	preempt_disable();
	queue_dequeue(queue, (void**) &next_thread);
	        printf("previous %d and state %d, next %d and state %d\n", previous_thread->id, previous_thread->state, next_thread->id, next_thread->state);
	//current_thread = next_thread;
	//current_thread->state = RUNNING;
	if (previous_thread->state != BLOCKED && previous_thread->state != DONE)  {//if blocked, the thread is stored in a semaphore blocked queue
	                previous_thread->state = READY;	
	}
	//printf("%d is being requeued\n", previous_thread->id);
	if (previous_thread->state != DONE)
		queue_enqueue(queue, previous_thread);
	//}
	//else {
	//	printf("%d is blocked\n", previous_thread->id);
	//}
	if (idle_thread == next_thread && queue_length(queue) > 1) {
		if (previous_thread->state != BLOCKED)
			current_thread->state = READY;
		queue_enqueue(queue, next_thread);
		queue_dequeue(queue, (void**) &next_thread);
		//current_thread = next_thread;
		//current_thread->state = RUNNING;
	} // currently not working, 41 is still running after being blocked
	while(next_thread->state == BLOCKED) { //iterate over all next threads until we find a unblocked thread
		printf("%d is blocked, moving on\n", next_thread->id);
//		current_thread->state = READY;
		queue_enqueue(queue, next_thread);
		queue_dequeue(queue, (void**) &next_thread);
		//current_thread = next_thread;
	}
	current_thread = next_thread;
	current_thread->state = RUNNING;
	printf("previous %d and state %d, next %d and state %d\n", previous_thread->id, previous_thread->state, next_thread->id, next_thread->state);
	uthread_ctx_switch(previous_thread->context, next_thread->context);
	preempt_enable();
}

void uthread_exit(void) {
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
	thread->state = READY;
	thread->id = tid;
	tid++;
	thread->context = malloc(sizeof(uthread_ctx_t));
	if (thread->stack_ptr == NULL || thread->context == NULL) {
		return -1;
	}
	if (uthread_ctx_init(thread->context, thread->stack_ptr,func, arg) == -1)
		return -1;
	preempt_disable();
	queue_enqueue(queue, thread);
	preempt_enable();
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	preempt_enable();
	queue = queue_create();
	preempt_disable();
	idle_thread = malloc(sizeof(struct uthread_tcb));
	if (idle_thread == NULL) {
		return -1;
	}
	idle_thread->context = malloc(sizeof(uthread_ctx_t));
	if (idle_thread->context == NULL) {
		return -1;
	}
	//current_thread->id = 1;
	tid++;
	current_thread = idle_thread;
	current_thread->state = RUNNING;
	preempt_start(preempt);
	if (uthread_create(func, arg) == -1) {
		return -1;
	}
	//uthread_yield();
	while (queue_length(queue) != 0) {	
		//printf("waiting\n");
		uthread_yield();
	}
	preempt_stop();
	free(idle_thread->context);
	uthread_ctx_destroy_stack(idle_thread->stack_ptr);
	free(idle_thread);
	return 0;
}

void uthread_block(void)
{
	printf("%d blocked\n", current_thread->id);
	current_thread->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
	//preempt_disable();
	//printf("enqueuing %d\n", uthread->id);
	//queue_enqueue(queue, uthread);
	//preempt_enable();
}

