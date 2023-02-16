ifeq ($(OS),Windows_NT)
	CC 			= clang
	DELETE_CMD 	= del
	BIN_FILE    = my_test.exe
else 
	CC          = gcc
	DELETE_CMD 	= -rm
	BIN_FILE    = my_test
endif

my_test: libtest.so my_test.o 
	$(CC) my_test.o ./libtest.so -o my_test 

my_test.o: my_test.c
	$(CC) -c my_test.c

.PHONY : run clean

run:
	make clean
	make
	./my_test

clean:
	$(DELETE_CMD) my_test.o $(BIN_FILE) test 2> del.log
	$(DELETE_CMD) del.log