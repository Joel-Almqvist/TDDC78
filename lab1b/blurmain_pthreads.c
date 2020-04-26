#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
#include <pthread.h>

#define MAX_RAD 1000

// rm blurpt
// make blurpt
// ./blurpt 10 4 data/im1.ppm data/im1-filtered-pt.ppm
int main (int argc, char ** argv)
{

	int radius, xsize, ysize, colmax, nr_threads;
	pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
	struct timespec stime, etime;
	double w[MAX_RAD];

	/* Take care of the arguments */
	if (argc != 5)
	{
		fprintf(stderr, "Usage: %s radius #threads infile outfile\n", argv[0]);
		exit(1);
	}

	radius = atoi(argv[1]);
	if ((radius > MAX_RAD) || (radius < 1))
	{
		fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
		exit(1);
	}

	nr_threads = atoi(argv[2]);

	if( nr_threads > 128 || nr_threads < 1){
		fprintf(stderr, "Must use between 1 and 128 threads\n");
		exit(1);
	}

	clock_gettime(CLOCK_REALTIME, &stime);

	/* Read file */
	if (read_ppm (argv[3], &xsize, &ysize, &colmax, (char *) src) != 0)
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

	get_gauss_weights(radius, w);

	// Exec start time
	clock_gettime(CLOCK_REALTIME, &stime);


  blur_args* args = malloc(sizeof(blur_args) * nr_threads);
  const int step = ysize / nr_threads;

	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, nr_threads);

	pixel *buffer = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);

  // Generate arguments
  for(int i = 0; i < nr_threads; i++){
    if(i == nr_threads - 1){
      args[i] = (blur_args){xsize, ysize, radius, i*step, ysize, w,
        src, buffer, &barrier};
    }
    else{
      args[i] = (blur_args){xsize, ysize, radius, i*step, (i+1)*step, w,
        src, buffer, &barrier};
    }
  }

  pthread_t threads[nr_threads];
  int ret;
	//clock_gettime(CLOCK_REALTIME, &stime);

  for(int i = 0; i < nr_threads; i++){
    ret = pthread_create(&threads[i], NULL, blurfilter_pt, args+i);
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
	if(write_ppm (argv[4], xsize, ysize, (char *)src) != 0)
		exit(1);
}
