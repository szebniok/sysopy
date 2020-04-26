CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -lrt -pthread

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: main creator packer sender
	./main

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

creator: creator.o
	$(CC) $(CFLAGS) $^ -o $@

packer: packer.o
	$(CC) $(CFLAGS) $^ -o $@

sender: sender.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main creator packer sender