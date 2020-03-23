CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

macierz: macierz.o
	$(CC) $(CFLAGS) $^ -o $@

helper: helper.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) macierz helper

.PHONY: run
run: macierz
	./macierz lista 3 10 1 100