.PHONY: all
all: distances clean 
distances: distances.c
	gcc -O2 distances.c -o distances -lm -fopenmp 
clean: 
	rm -f $(distances) *.o
