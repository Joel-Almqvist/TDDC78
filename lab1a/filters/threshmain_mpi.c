#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ppmio.h"
#include "thresfilter.h"
#include <math.h>
#include "mpi.h"

#define MAX_RAD 1000

/* Idea of the program:
  1 - Send non overlapping (mostly) equal chunks to all processes
  2 - Each process calculate the sum of their partition
  3 - Return sum to root and calculate global sum / #pixel
  4 - Each process gets the global sum from root and filters their chunk
  5 - Combine chunks at root and let root print it
*/

// rm threshmpi
// make threshmpi
// mpirun -np 2 threshmpi data/im1.ppm data/im1-filtered.ppm
int main (int argc, char ** argv)
{


	int p;   // number of started MPI processes
	int rank;  // my rank
	MPI_Status status;
  MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &p );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	double start_time;
	if(rank == 0)
		start_time = MPI_Wtime();

	int colmax, xsize, ysize;
	int dim[3];

	// src is only used by rank 0 but needs to be declared for all
	pixel *src;
	if(rank == 0){
		src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
    if (argc != 3)
    {
      fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
      exit(1);
    }

    /* Read file */
    if(read_ppm (argv[1], &xsize, &ysize, &colmax, (char *) src) != 0)
      exit(1);

    if (colmax > 255)
    {
      fprintf(stderr, "Too large maximum color-component value\n");
      exit(1);
    }

    printf("Has read the image, calling filter\n");

			dim[0] = colmax;
			dim[1] = xsize;
			dim[2] = ysize;
	}

	MPI_Bcast(&dim, 3, MPI_INT, 0, MPI_COMM_WORLD);
	colmax = dim[0];
	xsize = dim[1];
	ysize = dim[2];


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

	// ***********************************************************''



	// The size of each processors partition of pixels
	int* sizes = malloc(sizeof(int) *(p-1));
	int* displacements = malloc(sizeof(int) *(p-1));

	// How many pixels will be sent to all partitions
	int* sizes_to_send = malloc(sizeof(int) *(p-1));

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

		if(i == 0){
			*sizes_to_send = 0;
		}
		else{
			*(sizes_to_send+i) = *(sizes+i);
		}
	};


	pixel* chunk;
  if(rank != 0){
    chunk = malloc(sizeof(pixel) * sizes[rank]);
  }


		// Scatter the image in non-overlapping chunks
	 MPI_Scatterv(src, sizes_to_send, displacements, pixel_mpi,
	 	chunk, sizes[rank], pixel_mpi, 0, MPI_COMM_WORLD);

  unsigned int my_sum = 0;
  unsigned int global_sum = 0;
  // TODO Fix this
  if(rank == 0){
    my_sum = threshfilter_sum(xsize, sizes[rank]/xsize, src);
  }
  else{
    my_sum = threshfilter_sum(xsize, sizes[rank]/xsize, chunk);
  }


  MPI_Reduce(&my_sum, &global_sum, 1, MPI_UNSIGNED, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_sum, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  if(rank == 0){
    threshfilter_exec(xsize, sizes[rank]/xsize, src, global_sum / (xsize*ysize));
  }
  else{
    threshfilter_exec(xsize, sizes[rank]/xsize, chunk, global_sum / (xsize*ysize));
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Gatherv(chunk, sizes_to_send[rank], pixel_mpi, src, sizes_to_send,
    displacements, pixel_mpi, 0, MPI_COMM_WORLD);


	if(rank == 0){
		printf("Writing output file\n");
		if(write_ppm (argv[2], xsize, ysize, (char *)src) != 0){
			MPI_Finalize();
			exit(1);
		}
	}
	if(rank == 0)
	printf("Total execution took %f seconds\n", MPI_Wtime() - start_time);
  MPI_Finalize();
}
