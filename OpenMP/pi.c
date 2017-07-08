#include <stdio.h>
#include <omp.h>

static long num_steps = 1000000000;
double step;

#define NUM_THREAD 4
#define PARALLEL 0
#define PAD 8 //Elimate false sharing (64 byte cacheline size)s

int main() {

	step = 1.0/(double) num_steps;
	int i;
	double pi=0.0;

	#ifdef PARALLEL

	int act_threads;
	double sum[NUM_THREAD][PAD];	
	omp_set_num_threads(NUM_THREAD);	
	#pragma omp parallel 
	{
		int i;
		int id = omp_get_thread_num();
		int req_threads = omp_get_num_threads();
		double x;
		if (id == 0) {		
			act_threads = req_threads;
		}
		for (i=id, sum[id][0]=0.0; i<num_steps; i+=req_threads) {
			x = (i+0.5) * step;
			sum[id][0] += 4.0/(1.0 + x*x);
		}
	}
	for (int i=0; i<act_threads; i++) {
		pi += step * sum[i][0];
	}

	// Serial code
	#else
		double sum;
		for (i=0; i<num_steps; i++) {
			x = (i+0.5) * step;
			sum += 4.0/(1.0 + x*x);		
		}
		pi = step * sum;
	#endif	
	printf("pi = %f\n",pi);
	return 0;
}
