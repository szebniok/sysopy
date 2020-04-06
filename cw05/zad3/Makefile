CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all
all: producer consumer

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

consumer: consumer.o
	$(CC) $(CFLAGS) $^ -o $@

producer: producer.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(obj) consumer producer
