# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
queue: queue.c queue.h
	gcc -Wall -Wextra -Werror -o queue queue.c

uthread: uthread.c uthread.h private.h queue.h
	gcc -Wall -Wextra -Werror -o uthread uthread.c

context: context.c private.h uthread.h
	gcc -Wall -Wextra -Werror -o context context.c

preempt: preempt.c private.h uthread.h
	gcc -Wall -Wextra -Werror -o preempt preempt.c

sem: sem.c sem.h queue.h private.h
	gcc -Wall -Wextra -Werror -o sem sem.c


libuthread.a: queue.o uthread.o context.o preempt.o sem.o
	ar rcs libuthread.a queue.o uthread.o context.o preempt.o sem.o


clean: 
	rm -f libuthread.a queue.o uthread.o
