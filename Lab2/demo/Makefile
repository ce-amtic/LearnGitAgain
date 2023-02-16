CC 			:= gcc
CFLAGS 		:= -O2 -lm

PROGRAM1 	:= program1
PROGRAM2 	:= program2

INCLUDE		:= userdef.h
OBJ  		:= program1.o program2.o
SHARED_OBJ 	:= libobfuscate.so

.PHONY: Demo
Demo: $(PROGRAM1) $(PROGRAM2)
	@echo Demo Compiled Finish

$(PROGRAM1): $(PROGRAM1).o $(SHARED_OBJ)
	@$(CC) $< ./$(SHARED_OBJ) $(CFLAGS) -o $@

$(PROGRAM2): $(PROGRAM2).o $(SHARED_OBJ)
	@$(CC) $< ./$(SHARED_OBJ) $(CFLAGS) -o $@

%.o: %.c $(INCLUDE)
	@$(CC) -c $< $(CFLAGS) -o $@

.PHONY: run
run:
	@make clean && make

.PHONY: clean
clean:
	@-rm $(OBJ) $(PROGRAM1) $(PROGRAM2) --force

.PHONY: check
check: input $(PROGRAM1) $(PROGRAM2)
	@./$(PROGRAM1) < input > output1
	@./$(PROGRAM2) < input > output2
	@if (( diff output1 output2 -q; ));\
	then \
		echo "Same outputfile\nTry again !";\
	else \
		echo "Congratulations !";\
	fi

#	if you want to check the outputfile, then remove the following command
	@-rm output1 output2

### another equivalent version as below

# CC 			= gcc
# CXXFLAGS 		= -O2 -lm
# DELETE_CMD 	= -rm

# SHARED_OBJ 	= libobfuscate.so

# all: $(SHARED_OBJ) program1.o program2.o
# 	@$(CC) -o program1 program1.o ./$(SHARED_OBJ) $(CXXFLAGS)
# 	@$(CC) -o program2 program2.o ./$(SHARED_OBJ) $(CXXFLAGS)

# program1.o: program1.c
# 	@$(CC) -c program1.c $(CXXFLAGS)

# program2.o: program2.c
# 	@$(CC) -c program2.c $(CXXFLAGS)

# $(SHARED_OBJ): obfuscate.c
# 	@$(CC) $(CXXFLAGS) -fPIC -shared obfuscate.c -o $(SHARED_OBJ)

# .PHONY : run
# run:
# 	make clean
# 	make

# .PHONY : clean
# clean:
# 	@$(DELETE_CMD) -f *.so *.o program1 program2
