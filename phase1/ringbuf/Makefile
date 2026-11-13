CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -fsanitize=address,undefined
LDFLAGS = -fsanitize=address,undefined

all: rb_test

rb_test: main.o ringbuf.o
	$(CC) $(LDFLAGS) main.o ringbuf.o -o rb_test

main.o: main.c ringbuf.h
	$(CC) $(CFLAGS) -c main.c -o main.o

ringbuf.o: ringbuf.c ringbuf.h
	$(CC) $(CFLAGS) -c ringbuf.c -o ringbuf.o

clean:
	rm -f main.o ringbuf.o rb_test

.PHONY: all clean