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

sigset_t sig; // hold a set of signals that can be blocked or unblocked
struct sigaction sa; // used to configure and manage signal actions for a process
struct itimerval timer; // used to specify the amount of time to wait before sending a signal

// This function handles the SIGVTALRM signal and calls uthread_yield() to allow the current thread to yield the CPU

void preempt_handler(int sigg) {
	if (sigg == SIGVTALRM)
		uthread_yield();
}

// This function disables preemption by blocking the SIGVTALRM signal by using sigprocmask
void preempt_disable(void) {
	sigemptyset(&sig);
	sigaddset(&sig, SIGVTALRM);
	sigprocmask(SIG_BLOCK, &sig, NULL);
}

// This function enables preemption by unblocking the SIGVTALRM signal by using sigprocmask
void preempt_enable(void) {
    sigemptyset(&sig);
    sigaddset(&sig, SIGVTALRM);
	sigprocmask(SIG_UNBLOCK, &sig, NULL);
}

/* This function starts preemption by setting up a SIGVTALRM signal handler, 
setting a timer to send the signal repeatedly, and enabling the signal */
void preempt_start(bool preempt) {
	// Set the SIGVTALRM signal handler to preempt_handler()
	if (preempt) {

		sa.sa_handler = preempt_handler;
		sigemptyset(&sa.sa_mask);  // clear the sa_mask signal set
		sa.sa_flags = 0;
        sigaction(SIGVTALRM, &sa, NULL); // set the signal action for SIGVTALRM to sa

		// set the intervals and delays of the timer
        timer.it_value.tv_sec = 0;
		timer.it_value.tv_usec = 1000000 / HZ; // 100 Hz
		timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 1000000 / HZ; // 100 Hz
		// set the timer to virtual mode and start it
        setitimer(ITIMER_VIRTUAL, &timer, NULL);
    }
    else {
		// if preemption is disabled, return without doing anything
        return;
    }
}

void preempt_stop(void) {
	preempt_disable();
	// set the sa_flags to SA_RESETHAND to reset the signal handler for SIGVTALRM to its default behavior
	sa.sa_flags = SA_RESETHAND;
	// reset the signal handler for SIGVTALRM to its default behavior
	sigaction(SIGVTALRM, &sa, NULL);
}