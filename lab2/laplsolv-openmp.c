
//-----------------------------------------------------------------------
// Serial program for solving the heat conduction problem
// on a square using the Jacobi method.
// Written by August Ernstsson 2015-2019
//-----------------------------------------------------------------------

#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>


double timediff(struct timespec *begin, struct timespec *end)
{
	double sec = 0.0, nsec = 0.0;
	if ((end->tv_nsec - begin->tv_nsec) < 0)
	{
		sec  = (double)(end->tv_sec  - begin->tv_sec  - 1);
		nsec = (double)(end->tv_nsec - begin->tv_nsec + 1000000000);
	} else
	{
		sec  = (double)(end->tv_sec  - begin->tv_sec );
		nsec = (double)(end->tv_nsec - begin->tv_nsec);
	}
	return sec + nsec / 1E9;
}



void arrcpy(double *dst, double *src, int len)
{
	for (int it = 0; it < len; it++)
		dst[it] = src[it];
}


int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		printf("Usage: %s [size] [maxiter] [tolerance] [#threads]\n", argv[0]);
		exit(1);
	}

	int size = atoi(argv[1]);
	int maxiter = atoi(argv[2]);
	double tol = atof(argv[3]);
	int nr_threads = atoi(argv[4]);

	if(nr_threads > 128){
		printf("Max threads are 128\n");
		exit(1);
	}

	printf("Size %d, max iter %d and tolerance %f.\n", size, maxiter, tol);

	int n = size;
	double T[n+2][n+2];
	double* error = (double*) malloc(sizeof(double) * nr_threads);
	double tot_error;

	struct timespec starttime, endtime;
	int nr_its;

	// Set boundary conditions and initial values for the unknowns
	for (int i = 0; i <= n+1; ++i)
	{
		for (int j = 0; j <= n+1; ++j)
		{
			if      (i == n+1)           T[i][j] = 2;
			else if (j == 0 || j == n+1) T[i][j] = 1;
			else                         T[i][j] = 0;
		}
	}

	clock_gettime(CLOCK_MONOTONIC, &starttime);



#pragma omp parallel num_threads(nr_threads) shared(T, error, tot_error, nr_its)
{

	if(nr_threads != omp_get_num_threads()){
		printf("Inconsistent thread amount\n");
		exit(1);
	}

	int rank = omp_get_thread_num();
	int size = n / nr_threads;
	int start = size * rank + 1;
	int end = size * (rank+1) + 1;

	if(rank == nr_threads - 1){
		end += n % nr_threads;
	}

	double buffer_top[n];
	double buffer_bot[n];
	double buffer_curr[n];


	double prev_val;
	for (int k = 0; k < maxiter; ++k)
	{
		error[rank] = -INFINITY;

		// Since we filter from [startline, end) this is 1 line below
		// our last line we solve.
		arrcpy(buffer_top, &T[end][1], n);

		arrcpy(buffer_bot, &T[start - 1][1], n);

		// Do not modify T before every thread has filled their buffer
		#pragma omp barrier

		// Loop for each of this thread's rows except the last one
		for (int i = start; i < end - 1; ++i)
		{

			// Apply the Jacobi algorithm to each element in this row
			for (int j = 1; j <= n; ++j)
			{
				prev_val = buffer_curr[j-1];

				buffer_curr[j-1] = (T[i][j-1] + T[i][j+1] + T[i+1][j] +
					buffer_bot[j-1]) / 4.0;

				error[rank] = fmax(error[rank], fabs(buffer_curr[j-1] - prev_val));
			}

			// We need the current row unmodified for the next iteration
			arrcpy(buffer_bot, &T[i][1], n);
			arrcpy(&T[i][1], buffer_curr, n);
		}


		// Special case for the last row due to the row above it being modified
		for (int j = 1; j <= n; ++j)
		{
			prev_val = buffer_curr[j-1];
			buffer_curr[j-1] = (T[end - 1][j-1] + T[end - 1][j+1] + buffer_top[j-1]
					+ buffer_bot[j-1]) / 4.0;
			error[rank] = fmax(error[rank], fabs(buffer_curr[j-1] - prev_val));
		}
		arrcpy(&T[end - 1][1], buffer_curr, n);


		#pragma omp barrier
		#pragma omp single
		{
			for(int i = 0; i < nr_threads; ++i){
				tot_error = fmax(tot_error, error[rank]);
			}
			nr_its = k;
			#pragma omp flush(tot_error)
		}

		if(tot_error < tol){
			break;
		}

	} // End of iteration loop

	}	// End of parallel part
	clock_gettime(CLOCK_MONOTONIC, &endtime);
	printf("Time: %f\n", timediff(&starttime, &endtime));
	printf("Number of iterations: %d\n", nr_its);
	printf("Temperature of element T(1,1): %.17f\n", T[1][1]);
	return 0;
}
