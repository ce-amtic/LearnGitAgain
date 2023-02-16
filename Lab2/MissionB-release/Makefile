CC 			:= gcc
CFLAGS 		:= -O2 -lm

PROGRAM1 	:= program1
PROGRAM2 	:= program2

INCLUDE		:= userdef.h
OBJ  		:= program1.o program2.o
SHARED_OBJ 	:= libobfuscate.so

GREEN		:= "\e[0;32m"
RED			:= "\e[0;31m"
NONE    	:= "\e[0m"

.PHONY: MissionB
Demo: $(PROGRAM1) $(PROGRAM2)
	@echo $(GREEN)MissionB Compiled Finish$(NONE)

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
check: input $(PROGRAM1) $(PROGRAM2) checker
	@./checker < input -ne 0
	@./$(PROGRAM1) < input > output1
	@./$(PROGRAM2) < input > output2
	@if (( diff output1 output2 -q ));\
	then \
		echo $(RED)"Same outputfile\nTry again !"$(NONE);\
	else \
		echo $(GREEN)"Congratulations !"$(NONE);\
	fi

#	if you want to check the outputfile, then remove the following command
	@-rm output1 output2
