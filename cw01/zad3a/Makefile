CC = gcc 
CFLAGS = -Wall -Werror -std=c11 -g

.PHONY: all
all: run_all_configurations

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
	echo "*** small ***" >> raport3a.txt
	make -s small >> raport3a.txt
	echo "" >> raport3a.txt
	echo "*** medium ***" >> raport3a.txt
	make -s medium >> raport3a.txt
	echo "" >> raport3a.txt
	echo "*** big ***" >> raport3a.txt
	make -s big >> raport3a.txt
	echo "" >> raport3a.txt
	echo "*** almost_same ***" >> raport3a.txt
	make -s almost_same >> raport3a.txt
	echo "" >> raport3a.txt
	echo "*** very_different ***" >> raport3a.txt
	make -s very_different >> raport3a.txt
	echo "" >> raport3a.txt
	echo "*** repeated ***" >> raport3a.txt
	make -s repeated >> raport3a.txt
	echo "" >> raport3a.txt


.PHONY: static_tests
static_tests:
	make clean
	make static
	echo "----------------" >> raport3a.txt
	echo "    STATIC   " >> raport3a.txt
	echo "----------------" >> raport3a.txt

	make -s run_all_tests

.PHONY: shared_tests
shared_tests: 
	make clean
	make shared
	echo "----------------" >> raport3a.txt
	echo "    SHARED   " >> raport3a.txt
	echo "----------------" >> raport3a.txt

	make -s run_all_tests

.PHONY: dynamic_tests
dynamic_tests: 
	make clean
	make dynamic
	echo "----------------" >> raport3a.txt
	echo "    DYNAMIC   " >> raport3a.txt
	echo "----------------" >> raport3a.txt

	make -s run_all_tests

.PHONY: run_all_configurations
run_all_configurations:
	rm -f raport3a.txt
	make -s static_tests
	make -s shared_tests
	make -s dynamic_tests