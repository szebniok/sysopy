CC = gcc 
CFLAGS = -Wall -Werror -std=c11 -g

.PHONY: all
all: run_all_tests

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

.PHONY: small
small:
	./main 1 create_table 1 compare_pairs smallA.txt:smallB.txt create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_block 0

.PHONY: medium
medium:
	./main 1 create_table 1 compare_pairs mediumA.txt:mediumB.txt create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_block 0

.PHONY: big
big:
	./main 1 create_table 1 compare_pairs bigA.txt:bigB.txt create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_block 0

.PHONY: almost_same
almost_same:
	./main 1 create_table 1 compare_pairs sameA.txt:sameB.txt create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_block 0

.PHONY: very_different
very_different:
	./main 1 create_table 1 compare_pairs differentA.txt:differentB.txt create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_block 0

.PHONY: repeated
repeated:
	./main 5 create_table 5 \
	compare_pairs bigA.txt:bigB.txt \
	compare_pairs differentA.txt:differentB.txt \
	compare_pairs mediumA.txt:mediumB.txt \
	compare_pairs sameA.txt:sameB.txt \
	compare_pairs smallA.txt:smallB.txt \
	create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 1 0 \
	remove_operation 1 0 \
	remove_operation 2 0 \
	remove_operation 2 0 \
	remove_operation 3 0 \
	remove_operation 3 0 \
	remove_operation 4 0 \
	remove_operation 4 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0 \
	compare_pairs bigA.txt:bigB.txt \
	compare_pairs differentA.txt:differentB.txt \
	compare_pairs mediumA.txt:mediumB.txt \
	compare_pairs sameA.txt:sameB.txt \
	compare_pairs smallA.txt:smallB.txt \
	create_blocks \
	remove_operation 0 0 \
	remove_operation 0 0 \
	remove_operation 1 0 \
	remove_operation 1 0 \
	remove_operation 2 0 \
	remove_operation 2 0 \
	remove_operation 3 0 \
	remove_operation 3 0 \
	remove_operation 4 0 \
	remove_operation 4 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0 \
	remove_block 0

.PHONY: run_all_tests
run_all_tests:
	rm -f raport2.txt
	echo "*** small ***" >> raport2.txt
	make -s small >> raport2.txt
	echo "" >> raport2.txt
	echo "*** medium ***" >> raport2.txt
	make -s medium >> raport2.txt
	echo "" >> raport2.txt
	echo "*** big ***" >> raport2.txt
	make -s big >> raport2.txt
	echo "" >> raport2.txt
	echo "*** almost_same ***" >> raport2.txt
	make -s almost_same >> raport2.txt
	echo "" >> raport2.txt
	echo "*** very_different ***" >> raport2.txt
	make -s very_different >> raport2.txt
	echo "" >> raport2.txt
	echo "*** repeated ***" >> raport2.txt
	make -s repeated >> raport2.txt
	echo "" >> raport2.txt
