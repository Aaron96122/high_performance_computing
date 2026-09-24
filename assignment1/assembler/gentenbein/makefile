.PHONY: all run assembly
PROGRAMS = program_O0 program_O1 program_O2 program_O3 program_Os program_Og
ASSEMBLY = program_O0.s program_O1.s program_O2.s program_O3.s program_Os.s program_Og.s

all: $(PROGRAMS)
program_O0: naive.c
	gcc -O0 naive.c -o program_O0
program_O1: naive.c
	gcc -O1 naive.c -o program_O1
program_O2: naive.c
	gcc -O2 naive.c -o program_O2
program_O3: naive.c
	gcc -O3 naive.c -o program_O3
program_Os: naive.c
	gcc -Os naive.c -o program_Os
program_Og: naive.c
	gcc -Og naive.c -o program_Og

run: all
	@ : > benchmark_results.txt
	@ for program in $(PROGRAMS); do \
		echo "====$$program====" >> benchmark_results.txt; \
		./$$program >> benchmark_results.txt 2>&1; \
		echo >> benchmark_results.txt; \
	done

assembly: $(ASSEMBLY)
program_O0.s: naive.c
	gcc -S -fverbose-asm -O0 naive.c -o program_O0.s
program_O1.s: naive.c
	gcc -S -fverbose-asm -O1 naive.c -o program_O1.s
program_O2.s: naive.c
	gcc -S -fverbose-asm -O2 naive.c -o program_O2.s
program_O3.s: naive.c
	gcc -S -fverbose-asm -O3 naive.c -o program_O3.s
program_Os.s: naive.c
	gcc -S -fverbose-asm -Os naive.c -o program_Os.s
program_Og.s: naive.c
	gcc -S -fverbose-asm -Og naive.c -o program_Og.s