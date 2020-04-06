CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: run

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

consumer: consumer.o
	$(CC) $(CFLAGS) $^ -o $@

producer: producer.o
	$(CC) $(CFLAGS) $^ -o $@

main: main.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) consumer producer main *.txt pipe

.PHONY: setup_input
setup_input:
	echo -n "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" > A.txt
	echo -n "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB" > B.txt
	echo -n "1111111111111111111111111111111111111111111111111111111111111111111111" > 1.txt
	echo -n "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" > p.txt
	echo -n "......................................................................" > d.txt

.PHONY: run
run: producer consumer main setup_input
	./main