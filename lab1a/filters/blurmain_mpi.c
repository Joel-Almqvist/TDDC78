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
// mpirun -np 2 blurmpi 2 data/im1.ppm data/im1-filtered.ppm
int main (int argc, char ** argv)
{

	int p;   // number of started MPI processes
	int rank;  // my rank
	MPI_Status status;
  MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &p );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	double start_time;

	int colmax, xsize, ysize, radius;
	int dim[3];

	// src is only used by rank 0 but needs to be declared for all
	pixel *src;
	if(rank == 0){
		src = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);
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

			dim[0] = radius;
			dim[1] = xsize;
			dim[2] = ysize;
	}

	if(rank == 0)
		start_time = MPI_Wtime();


	MPI_Bcast(&dim, 3, MPI_INT, 0, MPI_COMM_WORLD);
	radius = dim[0];
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
	if(rank == p - 1){
		// We only need size for 1 margin
		chunk = (pixel*) malloc(sizeof(pixel) *
			(sizes[rank] + floor(radius) * xsize));

	} else if (rank != 0){

		chunk = (pixel*) malloc(sizeof(pixel) *
			(sizes[rank] + 2 * floor(radius) * xsize));

	}

		// Scatter the image in non-overlapping chunks
	 MPI_Scatterv(src, sizes_to_send, displacements, pixel_mpi,
	 	chunk + (int)(xsize*floor(radius)), sizes[rank], pixel_mpi, 0, MPI_COMM_WORLD);


	int margin = floor(radius)*xsize;

	MPI_Request req_prefix;
	MPI_Request req_suffix;

	if(rank == 0){
		pixel* start;
		pixel* end;
		for(int i = 1; i < p; i++){

			MPI_Request rq1;
			start = src + displacements[i] - margin;
			MPI_Isend(start, margin, pixel_mpi, i, 1111, MPI_COMM_WORLD, &rq1);

			// The last chunk does not have a lower margin
			if(i != p -1){
				MPI_Request rq2;
				end = src + displacements[i+1];
				MPI_Isend(end, margin, pixel_mpi, i, 2222, MPI_COMM_WORLD, &rq2);
			}
		}
	}
	else{
		MPI_Status status_pre;
		MPI_Irecv(chunk, margin, pixel_mpi, 0, 1111, MPI_COMM_WORLD,
			 &req_prefix);

			 // The last chunk does not have lower margin
			 if(rank != p - 1){
				 MPI_Status status_post;
				 MPI_Irecv(chunk + sizes[rank]+margin, margin, pixel_mpi, 0, 2222, MPI_COMM_WORLD,
					 &req_suffix);
			 }
	}
	double w[MAX_RAD];
	get_gauss_weights(radius, w);

	if(rank == 0){
		blurfilter(xsize, (sizes[rank]+margin)/xsize, src, radius, w, 0,
			(sizes[rank]+margin)/xsize);
	}
	else{
		MPI_Status status_pre;
		MPI_Wait(&req_prefix, &status_pre);

		if(rank != p - 1){
			MPI_Status status_post;
			MPI_Wait(&req_suffix, &status_post);

			int foo;

			MPI_Get_count(&status_post, pixel_mpi, &foo);

			printf("Rank %i got %i pixels as the post margin\n", rank, foo);


			blurfilter(xsize, (2*margin+sizes[rank])/xsize, chunk, radius, w, 0,
			 	(2*margin+sizes[rank])/xsize);
		}
		else{
			blurfilter(xsize, (margin+sizes[rank])/xsize, chunk, radius, w, 0,
			(margin+sizes[rank])/xsize);
		}
	}

	// Make sure that every process is done before we gather the data
	MPI_Barrier(MPI_COMM_WORLD);

	// Gather chunks without their margin from all non root processes
	MPI_Gatherv(chunk+margin, sizes_to_send[rank], pixel_mpi, src, sizes_to_send, displacements,
			pixel_mpi, 0, MPI_COMM_WORLD);

	printf("Total execution took %f seconds\n", MPI_Wtime() - start_time);

	if(rank == 0){
		printf("Writing output file\n");
		if(write_ppm (argv[3], xsize, ysize, (char *)src) != 0){
			MPI_Finalize();
			exit(1);
		}
	}
	if(rank == 0)
MPI_Finalize();
}
