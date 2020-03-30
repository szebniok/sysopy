CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -lrt

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: main child

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

child: child.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main child