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

queue_t queue;
static struct uthread_tcb* current_thread; // pointer to the currently executing thread
static struct uthread_tcb* idle_thread; // pointer to the idle thread


struct uthread_tcb {
	int state; /* 0 indicates ready to run, 1 indicates running, ,2 indicate blocked, 3 indicates done*/
	void* stack_ptr; // stores stack of the thread
	uthread_ctx_t* context; // stores the context/registers of the thread 
};

struct uthread_tcb *uthread_current(void)
{
	return current_thread; // returns a pointer to the current thread 
}

void uthread_yield(void) 
{
	// store the pointer to the currently running thread in preparation to yield
	struct uthread_tcb* previous_thread = uthread_current();
	struct uthread_tcb* next_thread = NULL;
	preempt_disable();
	// store pointer to the next thread in the queue
	queue_dequeue(queue, (void**) &next_thread);
	// check if the previous thread is not blocked or done, if so, do not change the state
	if (previous_thread->state != BLOCKED && previous_thread->state != DONE)  {
	                previous_thread->state = READY;	
	}
	// if the previous thread is not done, then we can enqueue it
	// if the previous thread is done, it was deleted in uthread_exit, there is no need to requeue
	if (previous_thread->state != DONE)
		queue_enqueue(queue, previous_thread);
	// check if the idle thread is the next thread, if so we can go to the next available thread
	if (idle_thread == next_thread && queue_length(queue) > 1) {
		// check once again that the previous thread is not blocked and set state to ready if so
		if (previous_thread->state != BLOCKED)
			current_thread->state = READY;
		// get next available thread
		queue_enqueue(queue, next_thread);
		queue_dequeue(queue, (void**) &next_thread);
	}
	// iterate until an unblocked thread is found in the queue
	while(next_thread->state == BLOCKED) { 
		queue_enqueue(queue, next_thread);
		queue_dequeue(queue, (void**) &next_thread);
	}
	// update the current thread pointer and switch contexts
	current_thread = next_thread;
	current_thread->state = RUNNING;
	uthread_ctx_switch(previous_thread->context, next_thread->context);
	preempt_enable();
}

void uthread_exit(void) {
	// set state of the thread to DONE and yield to the next available thread
	current_thread->state = DONE;
	struct uthread_tcb* done_thread = uthread_current();
	uthread_yield();
	// delete finished thread from queue, and free its contents
	queue_delete(queue, done_thread);
	uthread_ctx_destroy_stack(done_thread->stack_ptr);
	free(done_thread);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// instantiate a thread and its contents while checking for malloc errors
	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));
	if (thread == NULL) {
		return -1;
	}
	thread->stack_ptr = uthread_ctx_alloc_stack();
	thread->state = READY;
	thread->context = malloc(sizeof(uthread_ctx_t));
	if (thread->stack_ptr == NULL || thread->context == NULL) {
		return -1;
	}
	if (uthread_ctx_init(thread->context, thread->stack_ptr,func, arg) == -1)
		return -1;
	preempt_disable();
	// enqueue new thread
	queue_enqueue(queue, thread);
	preempt_enable();
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// create new queue
	preempt_enable();
	queue = queue_create();
	preempt_disable();
	// instantiate the idle thread
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
	preempt_start(preempt);
	// create a new thread
	if (uthread_create(func, arg) == -1) {
		return -1;
	}
	// run idle loop while the queue is not empty
	while (queue_length(queue) != 0) {	
		uthread_yield();
	}
	preempt_stop();
	// free the idle thread and queue after idle loop ends
	queue_destroy(queue);
	free(idle_thread->context);
	uthread_ctx_destroy_stack(idle_thread->stack_ptr);
	free(idle_thread);
	return 0;
}

void uthread_block(void)
{
	// set state of current thread to blocked so it will be skipped over
	// yield to next available thread in the queue
	current_thread->state = BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	// set state of specified thread to ready
	uthread->state = READY;
}

