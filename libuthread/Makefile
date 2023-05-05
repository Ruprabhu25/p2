# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
queue: queue.c queue.h
	gcc -Wall -Wextra -Werror -o queue queue.c

uthread: uthread.c uthread.h context.h preempt.h queue.h
	gcc -Wall -Wextra -Werror -o uthread uthread.c

context: context.c context.h preempt.h uthread.h
	gcc -Wall -Wextra -Werror -o context context.c

preempt: preempt.c preempt.h uthread.h
	gcc -Wall -Wextra -Werror -o preempt preempt.c


libuthread.a: queue.o uthread.o
	ar rcs libuthread.a queue.o uthread.o


clean: 
	rm -f libuthread.a queue.o uthread.o
