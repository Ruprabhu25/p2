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

int thread1_done = 0;
int thread2_done = 0;
int thread3_done = 0;
void thread3(void *arg)
{
	(void)arg;
	printf("thread3 loops\n");
	thread3_done = 1;
	uthread_yield();
}

void thread2(void *arg)
{
	(void)arg;

//	uthread_create(thread3, NULL);
	printf("thread2 loops\n");
	thread2_done = 1;
	while (1) {
		//printf("forever\n");
	}
        printf("this shouldn't be displayed\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	printf("thread1 loops\n");
	uthread_yield();
	thread1_done = 1;
	printf("this should display\n");
}


int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}
