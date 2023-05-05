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

int cur_id = 1;
queue_t queue;
struct uthread_tcb* current_thread;


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
void get_next(queue_t queue, void* node, void* arg) {
	struct uthread_tcb* thread = node;
	if (current_thread->id = thread->id) {
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

}

void uthread_yield(void)
{
	queue_func_t next_func = &get_next;
	queue_iterate(queue, get_next);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)

{
	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));
	thread->stack_ptr = uthread_ctx_alloc_stack();
	thread->state = RUNNING;
	thread->context = malloc(sizeof(uthread_ctx_t));
	uthread_ctx_init(thread->context, thread->stack_ptr,func, arg);
	queue_enqueue(queue, thread);
	/* TODO Phase 2 */
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	queue = queue_create();
	uthread_create(func, arg);
	return 0;

}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

