CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: run_all_tests

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main: $(obj)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) main wyniki.txt tmp tmp2

.PHONY: run
run: main
	./main

.PHONY: run_test
run_test: main
	echo "====================================" >> wyniki.txt
	echo "   RECORD_SIZE $(size)              " >> wyniki.txt
	echo "====================================" >> wyniki.txt

	echo "+++ RECORDS_COUNT $(count1) +++" >> wyniki.txt
	./main generate tmp $(count1) $(size)

	echo "*** Standard library ***" >> wyniki.txt
	./main copy tmp tmp2 $(count1) $(size) lib >> wyniki.txt
	./main sort tmp2 $(count1) $(size) lib >> wyniki.txt
	echo "*** Syscalls ***" >> wyniki.txt >> wyniki.txt
	./main copy tmp tmp2 $(count1) $(size) sys >> wyniki.txt
	./main sort tmp2 $(count1) $(size) sys >> wyniki.txt

	echo "+++ RECORDS_COUNT $(count2) +++" >> wyniki.txt
	./main generate tmp $(count2) $(size)

	echo "*** Standard library ***" >> wyniki.txt
	./main copy tmp tmp2 $(count2) $(size) lib >> wyniki.txt
	./main sort tmp2 $(count2) $(size) lib >> wyniki.txt
	echo "*** Syscalls ***" >> wyniki.txt >> wyniki.txt
	./main copy tmp tmp2 $(count2) $(size) sys >> wyniki.txt
	./main sort tmp2 $(count2) $(size) sys >> wyniki.txt

.PHONY: run_all_tests
run_all_tests: main
	rm -f wyniki.txt
	make run_test size=1 count1=10000 count2=25000
	make run_test size=2 count1=10000 count2=25000
	make run_test size=512 count1=10000 count2=25000
	make run_test size=1024 count1=10000 count2=25000
	make run_test size=4096 count1=10000 count2=25000