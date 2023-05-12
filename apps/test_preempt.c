/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>


void thread2(void *arg)
{
	(void)arg;

	printf("thread2 loops\n");
	while (1) {}
        printf("this statement should not print\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	printf("thread1 yields to thread2\n");
	uthread_yield();
	printf("thread1 done, exiting\n");
	exit(0);
}


int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
