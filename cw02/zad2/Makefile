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
	rm -rf test

.PHONY: run
run: main
	./main

.PHONY: setup_test_folders
setup_test_folders:
	rm -rf test
	mkdir test
	echo -n "0123456789" > test/tenbytes.txt
	mkdir test/directory
	echo "first level" > test/directory/first_level.txt
	mkdir test/directory/dir2
	echo "second level" > test/directory/dir2/second_level.txt
	ln -s dir2/second_level.txt test/directory/link1
	ln -s directory/dir2/second_level.txt test/link2