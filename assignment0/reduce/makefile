.PHONY: all
all : first_executable second_executable
first_executable : reduce.c
	gcc -o first_executable reduce.c

second_executable: reduce_alternative.c
	gcc -o second_executable reduce_alternative.c