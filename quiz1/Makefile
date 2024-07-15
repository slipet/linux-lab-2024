CC = gcc
CFLAGS = -O2 -g -w

all: main

OBJS := main.o list.o linux_list.o
deps := $(OBJS:%.o=.%.o.d)

main: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

$.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

test: main
	@./main
format:
	clang-format -i *.[ch]

clean:
	rm -f $(OBJS) $(deps)