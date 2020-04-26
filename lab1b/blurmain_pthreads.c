#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
#include <pthread.h>

#define MAX_RAD 1000
#define NR_THREADS 4

// rm blurpt
// make blurpt
// ./blurpt 2 data/im1.ppm data/im1-filtered-pt.ppm
int main (int argc, char ** argv)
{

	int radius, xsize, ysize, colmax;
	pixel *src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
  pixel *dest = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
	struct timespec stime, etime;
	double w[MAX_RAD];

	/* Take care of the arguments */
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s radius infile outfile\n", argv[0]);
		exit(1);
	}

	radius = atoi(argv[1]);
	if ((radius > MAX_RAD) || (radius < 1))
	{
		fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
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

	get_gauss_weights(radius, w);


  blur_args* args = malloc(sizeof(blur_args) * NR_THREADS);
  const int step = ysize / NR_THREADS;

	pthread_barrier_t barrier;

  // Generate arguments
  for(int i = 0; i < NR_THREADS; i++){
    if(i == NR_THREADS - 1){
      args[i] = (blur_args){xsize, ysize, radius, i*step, ysize, w,
        src, &barrier};
    }
    else{
      args[i] = (blur_args){xsize, ysize, radius, i*step, (i+1)*step - 1, w,
        src, &barrier};
    }
  }

  pthread_t threads[NR_THREADS];
  int ret;
	clock_gettime(CLOCK_REALTIME, &stime);

  for(int i = 0; i < NR_THREADS; i++){
    ret = pthread_create(&threads[i], NULL, blurfilter_pt, args+i);
    if(ret){
      printf("ERROR! Return code: %d\n", ret);
      exit(-1);
    }
  }

  for(int i = 0; i < NR_THREADS; i++){
    pthread_join(threads[i], NULL);
  }

  printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
  1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;
	clock_gettime(CLOCK_REALTIME, &etime);


	printf("Writing output file\n");
	if(write_ppm (argv[3], xsize, ysize, (char *)dest) != 0)
		exit(1);
}
