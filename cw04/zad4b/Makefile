CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: sender catcher

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

catcher: catcher.o
	$(CC) $(CFLAGS) $^ -o $@

sender: sender.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) catcher sender