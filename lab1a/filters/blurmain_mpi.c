#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "blurfilter.h"
#include "gaussw.h"
#include <math.h>
#include "mpi.h"

#define MAX_RAD 1000

// rm blurmpi
// make blurmpi
// mpirun -np 2 blurmpi 2 data/im1.ppm im1-filtered.ppm
int main (int argc, char ** argv)
{


	// TODO
	// 1 - Read file
	// 2 - Scatter file, ymax/p + 2*radius = number of rows
	//	use scatterv to send custom sized packets OR broadcast everything (don't)

	// Check what all the arguments in scatterv do


	// 3 - Filter ymax/p rows, just use the 2*radius rows for calculations
	// 4 - Combine all rows in correct order and write to file

	// TODO phreads
	// Load file twice, one read one for write
	// Write only your rows on write


	int p;   // number of started MPI processes
	int rank;  // my rank
	MPI_Status status;
  MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &p );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	int radius, xsize, ysize, colmax;

	// src is only used by rank 0 but needs to be declared for all
	pixel *src;
	if(rank == 0){
		src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
		//arr = malloc(sizeof(int) * 5);
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

			/* Read file */
			if (read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
				exit(1);

			if (colmax > 255)
			{
				fprintf(stderr, "Too large maximum color-component value\n");
				exit(1);
			}
			printf("Has read the image, generating coefficients\n");

	}

	// TODO combine these three by creating an array/struct of them
	MPI_Bcast(&xsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&ysize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&radius, 1, MPI_INT, 0, MPI_COMM_WORLD);

	printf("Rank %i got x: %i, y: %i, radius: %i\n", rank, xsize, ysize, radius);

	int* sizes = malloc(sizeof(int) *(p-1));
	int* displacements = malloc(sizeof(int) *(p-1));

	int base_size = (ysize / p)*xsize;
	for(int i = 0; i < p; i++){
		*(displacements+i) = base_size * i;
		if(i == p-1){
			// Add remaining rows to the last partition
			*(sizes+i) = base_size + (ysize % p)*xsize;
		}
		else{
			*(sizes+i) = base_size;
		}
	};



	printf("Rank %i got size %i\n", rank, sizes[rank]);

	// TODO Here we over allocate for the edge partitions
	pixel* chunk = (pixel*) malloc(sizeof(pixel) *
				(sizes[rank] + 2 * floor(radius) * xsize));


	printf("Allocated for %i pixels, with floor(radius) = %i\n",
	(int) (sizes[rank] + 2 * floor(radius) * xsize), (int)floor(radius));

	// Creating a MPI_Type ***********************************************
	MPI_Datatype pixel_mpi;
	pixel pxl;

	int block_lengths [] = {1, 1 ,1};

	MPI_Datatype block_types [] = { MPI_UNSIGNED_CHAR,
			MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR };

	MPI_Aint start, displ[3];

	MPI_Get_address( &pxl, &start );
	MPI_Get_address( &pxl.r, &displ[0] );
  MPI_Get_address( &pxl.g, &displ[1] );
  MPI_Get_address( &pxl.b, &displ[2] );

	displ[0] -= start;
	displ[1] -= start;
	displ[2] -= start;

	MPI_Type_create_struct(3, block_lengths, displ, block_types, &pixel_mpi);
	MPI_Type_commit(&pixel_mpi);

	// ***********************************

	printf("Added %i to chunk\n", ((int) (xsize*floor(radius))));
	printf("Size is %i\n", sizes[rank]);
	printf("Displacement is %i\n", displacements[rank]);
	printf("Sending %i and %i #elements\n", sizes[0], sizes[1]);

	 MPI_Scatterv(src, sizes, displacements, pixel_mpi,
	 	chunk + (int)(xsize*floor(radius)), sizes[rank], pixel_mpi, 0, MPI_COMM_WORLD);

	int margin = floor(radius)*xsize;
	if(rank == 0){
		pixel* start;
		pixel* end;
		for(int i = 1; i < p; i++){
			start = src + displacements[i] - margin;
			end = src + displacements[i] + sizes[i];

			MPI_Send(start, margin, pixel_mpi, i, 1111, MPI_COMM_WORLD);
			MPI_Send(end, margin, pixel_mpi, i, 2222, MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Status status_pre, status_post;
		// TODO Change these to non_block so we don't enforce unneeded order
		MPI_Recv(chunk, margin, pixel_mpi, 0, 1111, MPI_COMM_WORLD, &status_pre);
		MPI_Recv(chunk + sizes[rank], margin, pixel_mpi, 0, 2222, MPI_COMM_WORLD, &status_post);
	}

	MPI_Finalize();



	// else{
	// 	MPI_Recv(arr, 5, MPI_INT, 0, 0, communicator,
  //            MPI_STATUS_IGNORE);
	// }




// 	pixel *src_read;
// 	pixel *src_write;
//
// 	int radius, xsize, ysize, colmax;
// 	if(rank == 0){
//
// 	pixel *src_read = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
// 	pixel *src_write = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
// 	struct timespec stime, etime;
// 	double w[MAX_RAD];
//
// 	/* Take care of the arguments */
// 	if (argc != 4)
// 	{
// 		fprintf(stderr, "Usage: %s radius infile outfile\n", argv[0]);
// 		exit(1);
// 	}
//
// 	radius = atoi(argv[1]);
// 	if ((radius > MAX_RAD) || (radius < 1))
// 	{
// 		fprintf(stderr, "Radius (%d) must be greater than zero and less then %d\n", radius, MAX_RAD);
// 		exit(1);
// 	}
//
// 	/* Read file */
// 	if (read_ppm (argv[2], &xsize, &ysize, &colmax, (char *) src) != 0)
// 		exit(1);
//
// 	if (colmax > 255)
// 	{
// 		fprintf(stderr, "Too large maximum color-component value\n");
// 		exit(1);
// 	}
// 	printf("Has read the image, generating coefficients\n");
// }
//
// 	// TODO Scatter
// 	MPI_Scatter(src, MAX_PIXELS, )
//
//
// 	/* filter */
// 	get_gauss_weights(radius, w);
//
// 	printf("Calling filter\n");
//
//
// 	int partition = ysize / p;
// 	int ymin = partition * rank;
// 	int ymax;
// 	if(rank == p-1){
// 		// Always catch all rows even if the number of rows is not of modulu_p
// 		ymax = ysize;
// 	}
// 	else{
// 		ymax = partition * (rank+1);
// 	}
//
// 	clock_gettime(CLOCK_REALTIME, &stime);
// 	blurfilter(xsize, ysize, src, radius, w, ymin, ymax);
// 	clock_gettime(CLOCK_REALTIME, &etime);
//
// 	printf("Filtering took: %g secs\n", (etime.tv_sec  - stime.tv_sec) +
// 	1e-9*(etime.tv_nsec  - stime.tv_nsec)) ;
//
// 	/* Write result */
// 	printf("Writing output file\n");
//
// 	MPI_Finalize();
// 	if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0) {
// 		exit(1);
// 	}
}
