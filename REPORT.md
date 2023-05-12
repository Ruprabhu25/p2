# Project #2 - User-level thread library

## Summary
The objective of Project 2 is to gain an understanding
of a multi-threading library through the creation of a user-level one
called libuthread. This project aims to explore how multiple threads can
function within a single process, including the creation and termination
of threads, and the implementation of context switching during their
concurrent execution. The project includes several primary functionalities, 
such as cooperative thread creation, semaphore support, and preemption.

## FIFO Queue
First, we create a basic first in, first out queue class that can do the
following things: enqueue and dequeue elements, create and destroy a queue,
and a iterator function that you can use to apply a user defined function to 
an each element in the queue. To implement the queue class, we use a
singly-linked list as the data structure of the queue. This helps us achieve
doing O(1) when enqueuing and dequeuing. Singly linked lists can easily grow or 
shrink in size at runtime, unlike arrays which have a fixed size. This can be 
helpful when the number of elements in the queue is not known ahead of time.
We use this queue class in the other phases of the project such as when we work 
on the threading as we have to enqueue incoming threads. We also test our queue
using unit testing. We created a program called queue_tester.c and test all the
main features of the queue, such as adding multiple elements to the queue, 
removing elements from an empty queue, deleting elements from the queue, 
and iterating over the queue. These tests aim to ensure that the queue 
implementation works correctly and handles edge cases correctly.

## uthread API
To create the libuthread library, we first needed to implement the uthread api. 
The aim of the uthread api is to offer applications an interface, consisting of
a set of library functions, that enables them to create, launch, end, or control 
threads in various ways, for applications that require multithreading. We first 
defined what was needed in the struct for the uthread.

```
struct uthread_tcb {
	int state; 
	void* stack_ptr;
	uthread_ctx_t* context;
};
```

We chose these to be in the structure since it holds all the necessary information
for a thread's context and for managing its lifecycle, such as creating new threads,
switching to another thread, blocking a thread, and deleting a thread.

We first work on creating uthread_run() as that is where the execution starts
in the test cases. The uthread_run function initializes the thread library by 
creating a queue, creating the idle_thread, and setting it as the current_thread.
It then creates a new thread with the given function and argument. The function then 
loops until the queue is empty. 


## Semaphore 
The semaphore implementation was straightforward: the semaphore that was
implemented only contains a queue of blocked threads that have requested 
its "resource" and the internal
count of how many "resources" it has. sem_create() and sem_destroy() only 
instantiate and free the memory allocated to the semaphore, but sem_up() and 
sem_down() required the uthread functions as well. By theory, a thread gets
blocked when there are no more free resources in the semaphore, so when 
sem_down() was called, we decided to check if the internal count was 0 and then
add it to the blocked queue if so. This helped in the implementation of sem_up,
first checking if there were any blocked threads and dequeuing the first or 
oldest blocked thread if so. The uthread queue would also have to be affected,
so we implemented a blocked state in our uthread queue. When calling the
uthread_block() and uthread_unblock in sem_down() and sem_up(), we would change
the state of the thread in the uthread queue to blocked and ready. If a thread
was blocked, then when uthread_yield was called, it would then check if a thread
was blocked and skip over as many blocked threads until it found a ready thread.



## Preemption

The code in preempt.c implements preemption in a multi-threaded environment 
using signals and timers. When we test our threading, we can set the preempt
variable of uthread_run(), which is a boolean, to true, which will allow us
to use the preempt_start and preempt_disable functions in our threads. The 
preempt_start function sets up the timer and signal handler by registering 
the signal handler and configuring the timer using the setitimer function. When 
preemption is enabled, the timer is started, and the signal handler is activated 
periodically to invoke the uthread_yield function, which causes the current thread 
to relinquish the CPU and give other threads a chance to run. The preempt_stop() 
function stops the timer and resets the signal handler to its previous state. Also 
there is preempt_disable and preempt_enable, which use the sigprocmask() function, 
which can either block or unblock the SIGALARM or SIGVTALARM. We created a testing
file called test_preempt.c which calls the uthread_run() function with preempt to true.
This makes thread1 yield to thread2, but since thread2 is running infinite, the preempt 
alarm handler goes off after a certain amount of time which causes it to yield back
to thread1. Since thread2 never called yield and just goes back to thread1, we can 
confirm that our preemption implementation works as it uses the timer instead.