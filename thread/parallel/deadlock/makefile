BINS = deadlock_simple deadlock_dependent
CFLAGS = -O2 -lpthread

.PHONY : all
all : $(BINS) 

deadlock_simple : deadlock_simple.c
	gcc $(CFLAGS) -o $@ $<

deadlock_dependent : deadlock_dependent.c
	gcc $(CFLAGS) -o $@ $<

.PHONY : clean
clean :
	rm -rf $(BINS)