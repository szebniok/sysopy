CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: $(obj)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main

.PHONY: run
run: main
	./main