CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -pthread

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: server client
	./server 1234 /tmp/socket

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

client: client.o
	$(CC) $(CFLAGS) $^ -o $@

server: server.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) client server