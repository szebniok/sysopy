CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -lrt

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: server client

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

server: server.o
	$(CC) $(CFLAGS) $^ -o $@

client: client.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) client server