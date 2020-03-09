CC = gcc 
CFLAGS = -Wall -Werror -std=c11 -g

.PHONY: all
all: static

.PHONY: static
static:
	$(CC) $(CFLAGS) -c library.c -o library.o
	ar rcs liblibrary.a library.o
	$(CC) $(CFLAGS) main.c -llibrary -L./ -o main

.PHONY: shared
shared:
	$(CC) $(CFLAGS) -fPIC -shared library.c -o liblibrary.so
	$(CC) $(CFLAGS) main.c -L. -Wl,-rpath=. -llibrary -o main

.PHONY: dynamic
dynamic:
	$(CC) $(CFLAGS) -fPIC -shared library.c -o liblibrary.so
	$(CC) $(CFLAGS) main.c -ldl -D DYNAMIC -o main

.PHONY: clean
clean: 
	rm -f *.o *.a *.so .tmp main