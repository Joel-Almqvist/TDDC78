#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "thresfilter.h"
#include "gaussw.h"
#include <pthread.h>

#define MAX_RAD 1000

// rm blurpt
// make blurpt
// ./threshpt 10 data/im1.ppm data/im1-threshed-pt.ppm
int main (int argc, char ** argv)
{

	int xsize, ysize, colmax, nr_threads;
	pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
	struct timespec stime, etime;
	double w[MAX_RAD];

	/* Take care of the arguments */
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s #threads infile outfile\n", argv[0]);
		exit(1);
	}

	nr_threads = atoi(argv[1]);
	if( nr_threads > 128 || nr_threads < 1){
		fprintf(stderr, "Must use between 1 and 128 threads\n");
		exit(1);
	}

	clock_gettime(CLOCK_REALTIME, &stime);

	/* Read file */
	if (read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
		exit(1);

	clock_gettime(CLOCK_REALTIME, &etime);
		printf("Reading file took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
		1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;


	if (colmax > 255)
	{
		fprintf(stderr, "Too large maximum color-component value\n");
		exit(1);
	}

	printf("Has read the image, generating coefficients\n");


	// Exec start time
	clock_gettime(CLOCK_REALTIME, &stime);


  thresh_sum_args* args = malloc(sizeof(thresh_sum_args) * nr_threads);
  const int step = ysize / nr_threads;

	uint* return_vals = malloc(sizeof(uint) * nr_threads);

  // Generate arguments for sum
  for(int i = 0; i < nr_threads; i++){
    if(i == nr_threads - 1){
      args[i] = (thresh_sum_args){xsize, i*step, ysize,
        src, return_vals+i};
    }
    else{
			args[i] = (thresh_sum_args){xsize, i*step, (i+1)*step,
				src, return_vals+i};
    }
  }

  pthread_t threads[nr_threads];
  int ret;

	// Find image total sum
  for(int i = 0; i < nr_threads; i++){
    ret = pthread_create(&threads[i], NULL, threshfilter_sum, args+i);
    if(ret){
      printf("ERROR! Return code: %d\n", ret);
      exit(-1);
    }
  }

  for(int i = 0; i < nr_threads; i++){
    pthread_join(threads[i], NULL);
  }


	long unsigned int sum;
	for(int i = 0; i < nr_threads; i++){
    sum += return_vals[i];
  }

	sum /= xsize*ysize;
	thresh_exec_args* args_exec = malloc(sizeof(thresh_exec_args) * nr_threads);

	// Generate  arguments for exec
	for(int i = 0; i < nr_threads; i++){
    if(i == nr_threads - 1){
			args_exec[i] = (thresh_exec_args) {xsize, i*step, ysize, src, sum};
    }
    else{
			args_exec[i] = (thresh_exec_args) {xsize, i*step, (i+1)*step, src, sum};
    }
  }


	// Threshold image
	for(int i = 0; i < nr_threads; i++){
    ret = pthread_create(&threads[i], NULL, threshfilter_exec, args_exec+i);
    if(ret){
      printf("ERROR! Return code: %d\n", ret);
      exit(-1);
    }
  }

  for(int i = 0; i < nr_threads; i++){
    pthread_join(threads[i], NULL);
  }


	// Exec end time
	clock_gettime(CLOCK_REALTIME, &etime);


  printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
  1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;
	clock_gettime(CLOCK_REALTIME, &etime);


	printf("Writing output file\n");
	if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0)
		exit(1);
}
