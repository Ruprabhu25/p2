#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

#define _XOPEN_SOURCE 1

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

sigset_t sig;
struct sigaction sa;
struct sigaction old_sa;
struct itimerval timer;
struct itimerval old_timer;

void preempt_handler(int sigg) {
	if (sigg == SIGVTALRM)
		uthread_yield();
}

void preempt_disable(void) {
	sigprocmask(SIG_BLOCK, &sig, NULL);
}

void preempt_enable(void) {
	sigprocmask(SIG_UNBLOCK, &sig, NULL);

}

void preempt_start(bool preempt) {
	if (preempt) {
		sa.sa_handler = preempt_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
        	sigaction(SIGVTALRM, &sa, &old_sa);

        	timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 1000000 / HZ; // 100 Hz
		timer.it_interval.tv_sec = 0;
        	timer.it_interval.tv_usec = 1000000 / HZ; // 100 Hz
        	setitimer(ITIMER_VIRTUAL, &timer, NULL);

        	if (setitimer(ITIMER_VIRTUAL, &timer, &old_timer) == -1) { // save old configuration
        		perror("setitimer");
        		exit(1);
        	}
    	}
    	else {
        	return;
    	}
}

void preempt_stop(void) {
	preempt_disable();

	sigaction(SIGALRM, &old_sa, NULL);

	if (setitimer(ITIMER_REAL, &old_timer, NULL) == -1) {
		perror("setitimer");
        	exit(1);
    	}
}

