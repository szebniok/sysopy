CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: test_main test_nftw

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

nftw: nftw.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main nftw
	rm -rf test

.PHONY: run
run: main
	./main

.PHONY: test_main
test_main: setup_test_folders main
	./main test -maxdepth 3

.PHONY: test_nftw
test_nftw: setup_test_folders nftw
	./nftw test -maxdepth 3

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