CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g -lpthread

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: main
	./main 5 sign saturn.ascii.pgm saturn_result.ascii.pgm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main