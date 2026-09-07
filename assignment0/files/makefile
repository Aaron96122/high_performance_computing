.PHONY : all
all : stack_executable heap_continuous heap_fragment
stack_executable : file_stack.c
	gcc -o stack_executable file_stack.c
heap_continuous : file_heap.c
	gcc -o heap_continuous file_heap.c
heap_fragment : file_heap_frag.c
	gcc -o heap_fragment file_heap_frag.c