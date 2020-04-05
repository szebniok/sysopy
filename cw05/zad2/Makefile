CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main

.PHONY: run
run: main test.txt
	./main test.txt

test.txt:
	echo "D ta linia jest czwarta" >> test.txt
	echo "A ta linia jest pierwsza" >> test.txt
	echo "C ta linia jest trzecia" >> test.txt
	echo "B ta linia jest druga" >> test.txt