.PHONY: all benchmark
PROGRAMS = \
	same/same_file \
	different/different_file \
	inline/inlined_manually 

all: 
	$(MAKE) -C same
	$(MAKE) -C different
	$(MAKE) -C inline

benchmark: all
	@ : > benchmark_results.txt
	@ for program in $(PROGRAMS); do \
		echo "====$$program====" >> benchmark_results.txt; \
		./$$program >> benchmark_results.txt 2>&1; \
		echo >> benchmark_results.txt; \
	done
