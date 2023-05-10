# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o sem.o

CC := gcc
CFLAGS := -Wall -Wextra -Werror
CFLAGS += -g

ifneq ($(V),1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(lib): $(objs)
	@echo "create static library libuthread"
	$(Q)ar rcs $(lib) $(objs)

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)
