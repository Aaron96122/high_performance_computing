#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	const long N=1000000000;
	const int bench_iter=5;
	struct timespec bench_start_time;
	struct timespec bench_stop_time;
	long num=0;
	double bench_diff_time;
	timespec_get(&bench_start_time, TIME_UTC);
	for (int bx=0; bx<bench_iter; bx++)
	{
		num = 0;
		for (int ix=0; ix<N; ix++)
		{
			num += ix;
		}
	}
	printf("The result is %ld\n", num);
	timespec_get(&bench_stop_time, TIME_UTC);
	bench_diff_time =
	       difftime(bench_stop_time.tv_sec, bench_start_time.tv_sec) * 1000000 + (bench_stop_time.tv_nsec - bench_start_time.tv_nsec) / 1000;
	printf("benchmark time for one iteration: %f\n", bench_diff_time/bench_iter);
	return 0;	
	

}
