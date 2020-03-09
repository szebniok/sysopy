CC = gcc 
CFLAGS = -Wall -Werror -std=c11 -g

.PHONY: all
all: static shared

.PHONY: static
static:
	$(CC) $(CFLAGS) -c library.c -o library.o
	ar rcs liblibrary.a library.o

.PHONY: shared
shared:
	$(CC) $(CFLAGS) -fPIC -shared library.c -o liblibrary.so

.PHONY: clean
clean:
	rm -f *.so *.o *.a