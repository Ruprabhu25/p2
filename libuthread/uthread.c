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

 

int cur_id = 1;
queue_t queue;
struct uthread_tcb {
	/* TODO Phase 2 */
	int id;
	int state; /* 0 indicates ready to run, 1 indicates running, 2 indicate exit*/
	void * stack_ptr;
	uthread_ctx_t * context;

};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)

{
	struct uthread_tcb* thread = malloc(sizeof(struct uthread_tcb));
	thread->stack_ptr = uthread_ctx_alloc_stack();
	thread->state = 1;
	thread->context = malloc(sizeof(uthread_ctx_t));
	uthread_ctx_init(thread->context, thread->stack_ptr,func, arg);
	queue_enqueue(queue, thread);
	/* TODO Phase 2 */
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	queue = queue_create();

	uthread_create(func, arg);

}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

