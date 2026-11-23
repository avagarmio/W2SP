CC = gcc
CFLAGS = -MMD -MP -Wall -Wextra -std=c11 -g -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

all: rb_test

rb_test: main.o ringbuf.o
	$(CC) $(LDFLAGS) $^ -o $@

-include main.d ringbuf.d

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f main.o ringbuf.o rb_test main.d ringbuf.d

.PHONY: all clean