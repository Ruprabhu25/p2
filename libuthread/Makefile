# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
queue: queue.c queue.h
	gcc -Wall -Wextra -Werror -o queue queue.c 

libuthread.a: queue.o
	ar rcs libuthread.a queue.o

clean: 
	rm -f libuthread.a queue.o
