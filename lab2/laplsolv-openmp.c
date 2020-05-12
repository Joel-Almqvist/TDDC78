
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

void arrcpy(double *dst, double *src, int len)
{
	for (int it = 0; it < len; it++)
		dst[it] = src[it];
}


int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		printf("Usage: %s [size] [maxiter] [tolerance] \n", argv[0]);
		exit(1);
	}

	int size = atoi(argv[1]);
	int maxiter = atoi(argv[2]);
	double tol = atof(argv[3]);

	printf("Size %d, max iter %d and tolerance %f.\n", size, maxiter, tol);

	int n = size;

	double T[n+2][n+2];
	double tmp1[n], tmp2[n], tmp3[n];

	struct timespec starttime, endtime;


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

bool stop = false;
#pragma omp parallel num_threads(4) shared(stop, T)
{
	int rank = omp_get_thread_num();
	int nr_threads = omp_get_num_threads();


	int size = n / nr_threads;
	int start = size * rank + 1;
	int end = size * (rank+1) + 1;

	if(rank == nr_threads - 1){
		end += n % nr_threads;
	}

	printf("Id %i  - From row %i to %i\n", rank, start, end);


	//double tmp1[n], tmp2[n], tmp3[n];

	double buffer_top[n];
	double buffer_bot[n];
	double buffer_curr[n];

	for (int k = 0; k < maxiter; ++k)
	{
		#pragma omp barrier




		double error = -INFINITY;
		//arrcpy(tmp1, &T[start - 1][1], n);


		// Since we filter from [startline, end) this is 1 line below
		// our last line we solve.
		arrcpy(buffer_top, &T[end][1], n);

		arrcpy(buffer_bot, &T[start - 1][1], n);

		#pragma omp barrier

		// Loop for each of this thread's rows
		for (int i = start; i < end - 1; ++i)
		{

			// Apply the Jacobi algorithm to each element in this row
			for (int j = 1; j <= n; ++j)
			{
				buffer_curr[j-1] = (T[i][j-1] + T[i][j+1] + T[i+1][j] +
					buffer_bot[j-1]) / 4.0;
				//error = fmax(error, fabs(tmp2[j-1] - tmp3[j-1]));
			}

			//printf("\n [%i, %i) Printing to T[%i]\n", start, end, i);
			arrcpy(buffer_bot, &T[i][1], n);
			arrcpy(&T[i][1], buffer_curr, n);
		}

			//arrcpy(tmp2, &T[end - 1][1], n);
			//int i = end - 1;
			// Apply the Jacobi algorithm to each element in this row
			for (int j = 1; j <= n; ++j)
			{
				buffer_curr[j-1] = (T[end - 1][j-1] + T[end - 1][j+1] + buffer_top[j-1]
						+ buffer_bot[j-1]) / 4.0;
			}
				arrcpy(&T[end - 1][1], buffer_curr, n);
	}

	}	// End of parallel part
	clock_gettime(CLOCK_MONOTONIC, &endtime);
	printf("Temperature of element T(1,1): %.17f\n", T[1][1]);

	return 0;
}
