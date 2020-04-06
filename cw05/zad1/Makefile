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
	rm -f $(obj) main test.txt

.PHONY: run
run: main test.txt
	./main test.txt

test.txt:
	echo "ls -la . | grep main" >> test.txt
	echo "ls -la . | grep Makefile" >> test.txt
	echo "ls -la . | grep main | grep \.c" >> test.txt