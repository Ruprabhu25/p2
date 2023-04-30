# Target library
lib := libuthread.a

all: $(lib)

## TODO: Phase 1
ar rcs libuthread.a queue.o

queue: queue.c queue.h
	gcc -Wall -Wextra -Werror -o queue queue.c 

clean: 
	rm -f lib
