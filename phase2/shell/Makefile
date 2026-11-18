CC = gcc
CFLAGS = -MMD -MP -Wall -Wextra -std=c11 -g -fsanitize=address,undefined -Wshadow
LDFLAGS = -fsanitize=address,undefined

all: build/w2sp

build/w2sp: build/main.o
	$(CC) $(LDFLAGS) $^ -o $@

-include build/main.d

build:
	mkdir -p build

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean