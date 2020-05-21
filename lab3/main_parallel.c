#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <mpi.h>

#include "coordinate.h"
#include "definitions.h"
#include "physics.h"
#include "linked_list.h"
//Feel free to change this program to facilitate parallelization.

float rand1(){
	return (float)( rand()/(float) RAND_MAX );
}


int* denoms(int x){
	int* denom = malloc(sizeof(int) * 2);
	int n = 0;


	if(x == 4){
		denom[0] = 2;
		denom[1] = 2;
		return denom;
	}

	if(x == 2){
		denom[0] = 2;
		denom[1] = 1;
		return denom;
	}

	for(int i = x - 2; i > 1; --i){
		if(x % i == 0){
			denom[n] = i;
			if(++n == 2){
				break;
			}
		}
	}
	if(n != 2){
		denom[0] = 1;
	}
	return denom;
}


void set_box_limits(int* nr_rows_ret, int* nr_cols_ret,
		int nr_proc, int rank, double* border_top, double* border_right,
		double* border_bot, double* border_left, int* dest, int* nr_neighbors){

			int* den = denoms(nr_proc);
			if(den[0] == 1){
				printf("Unable to divide the task for %i processes\n", nr_proc);
				MPI_Finalize();
				exit(1);
			}

			int nr_rows = den[0];
			int nr_cols = den[1];

			double x_step = BOX_HORIZ_SIZE / (double) nr_cols;
			double y_step = BOX_VERT_SIZE / (double) nr_rows;


			// These are zero indexed
			int my_col = rank % nr_cols;
			int my_row = rank / nr_cols;


			double x_border_left = my_col * x_step;
			double x_border_right;

			if(my_col == nr_cols - 1){
				x_border_right = BOX_HORIZ_SIZE;
			}
			else{
				 x_border_right = (my_col + 1) * x_step;
			}

			double y_border_top = my_row * y_step;
			double y_border_bot;

			if(my_row == nr_rows - 1){
				y_border_bot = BOX_VERT_SIZE;
			}
			else{
				y_border_bot = (my_row + 1) * y_step;
			}

			*border_bot = y_border_bot;
			*border_top = y_border_top;
			*border_right = x_border_right;
			*border_left = x_border_left;

			*nr_rows_ret = nr_rows;
			*nr_cols_ret = nr_cols;



			// Find our neighbors in the grid
			*nr_neighbors = 0;
			if(my_row != 0){
				*nr_neighbors += 1;
				dest[0] = (my_row - 1) * nr_cols + my_col;
			}
			else{
				dest[0] = -1;
			}

			if(my_row != nr_rows - 1){
				*nr_neighbors += 1;
				dest[1] = (my_row + 1) * nr_cols + my_col;
			}
			else{
				dest[1] = -1;
			}

			if(my_col != 0){
				*nr_neighbors += 1;
				dest[2] = rank - 1;
			}
			else{
				dest[2] = -1;
			}

			if(my_col != nr_cols - 1){
				*nr_neighbors += 1;
				dest[3] = rank + 1;
			}
			else{
				dest[3] = -1;
			}


			free(den);
		}

/* Searches the list for collisions, removes any colliding elements
 * and adds them to the collision list.
 *
 * Also interacts two colliding elements
*/
plist_elem* search_collision(particle_list* particles, plist_elem* start,
	particle_list* collisions){

	bool collided = false;
	float t = -1;

	plist_elem* part = start->next;

	if(part == NULL){
		return NULL;
	}

	// Iterate over every following particle
	while(true){




		t = collide(&(start->this.pcord), &(part->this.pcord));

		if(t != -1){
			interact(&(start->this.pcord), &(part->this.pcord), t);

			plist_elem* next_elem;

			if(start->next == part){
				next_elem = part->next;
			}
			else{
				next_elem = start->next;
			}

			remove_particle(particles, start);
			remove_particle(particles, part);

			append(collisions, start);
			append(collisions, part);

			// We only collide once per iteration
			return next_elem;
		}

		if(part->next == NULL){
			break;
		}
		else{
			part = part->next;
		}
	}
	return start->next;
}

// mpirun -np 6 --oversubscribe par 4
int main(int argc, char** argv){
	unsigned int time_stamp = 0, time_max;
	double my_pressure = 0;
	double global_pressure = 0;

	// parse arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: %s simulation_time\n", argv[0]);
		fprintf(stderr, "For example: %s 10\n", argv[0]);
		exit(1);
	}

	time_max = atoi(argv[1]);


	int nr_proc;   // number of started MPI processes
	int rank;  // my rank
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &nr_proc );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );


	// buffer_size is a design variable
	int buffer_size = 2*(MAX_NO_PARTICLES / nr_proc);
	particle_t* rec_buff = malloc(buffer_size * sizeof(particle_t));
	particle_t* send_buff = malloc(buffer_size * sizeof(particle_t));
	MPI_Status status[4];
	MPI_Request	mpi_rq[4];

	int nr_rows;
	int nr_cols;
	int nr_neighbors;
	int dest[4];


	double border_right, border_left, border_top, border_bot;

	set_box_limits(&nr_rows, &nr_cols, nr_proc, rank, &border_top,
		&border_right, &border_bot, &border_left, dest, &nr_neighbors);

	// All particles (may change during an iteration)
	particle_list particles;
	init(&particles);

	// All colliding particles
	particle_list collisions;
	init(&collisions);

	// All particles that has left our box
	particle_list parts_to_send;
	init(&parts_to_send);

	int my_start_partition;
	if(rank == nr_proc - 1){
		my_start_partition = INIT_NO_PARTICLES / nr_proc + INIT_NO_PARTICLES % nr_proc;
	}
	else{
		my_start_partition = INIT_NO_PARTICLES / nr_proc;
	}


	srand(time(NULL));

	float x;
	float y;
	float vx;
	float vy;
	float theta;
	float absv;

	for(int i = 0; i <= my_start_partition; i++){

		x = ((float) rand() / (float) RAND_MAX) * (border_right - border_left) + border_right;
		y = ((float) rand() / (float) RAND_MAX) * (border_bot - border_top) + border_top;

		theta = ((float) rand()/ (float) RAND_MAX) * (2 * PI);
		absv = ((float) rand()/ (float) RAND_MAX) * 50;

		vx = absv * cos(theta);
		vy = absv* sin(theta);

		append(&particles, create_particle(x, y, vx, vy));
	}


	// Create the wall
	cord_t wall;
	wall.y0 = wall.x0 = 0;
	wall.x1 = BOX_HORIZ_SIZE;
	wall.y1 = BOX_VERT_SIZE;


	// Used to iterate through linked list
	plist_elem* part;
	for(int iteration = 0; iteration < time_max; ++iteration){

	part = particles.first;

	// Collide all elements and remove collided elements from particles
	while(true){
		part = search_collision(&particles, part,  &collisions);

		if(part == NULL){
			break;
		}
	}


	part = collisions.first;
	pcord_t* mcord;
	plist_elem* next_part;
	while(true){
		if(part == NULL){
			break;
		}
		mcord = &(part->this.pcord);

		feuler(mcord, 1);
		my_pressure += wall_collide(mcord, wall);

		// Removing an element from the list destroys its next reference
		next_part = part->next;

		// We empty collision list each iteration
		remove_particle(&collisions, part);

		if(mcord->y < border_top || mcord->y > border_bot || mcord->x < border_left
			|| mcord->x > border_right){
				// This particle has left our box, move it to send list
				append(&parts_to_send, part);
			}
		else{
			// Move particle back to particle list
			append(&particles, part);
		}

		part = next_part;
	}

	int offset = buffer_size / 4;
	size_t sizeb = sizeof(particle_t);
	int send_sizes[4] = {0,0,0,0};

	part = parts_to_send.first;
	while(true){
		if(part == NULL){
			break;
		}

		mcord = &(part->this.pcord);

		if(mcord->y < border_top){
			memcpy(send_buff + send_sizes[0], &part->this, sizeb);
			send_sizes[0] += 1;
		}
		else if(mcord->y > border_bot){
			memcpy(send_buff + offset + send_sizes[1], &part->this, sizeb);
			send_sizes[1] += 1;
		}
		else if( mcord->x < border_left){
			memcpy(send_buff + send_sizes[2] + 2 * offset, &part->this, sizeb);
			send_sizes[2] += 1;
		}
		else{
			memcpy(send_buff + send_sizes[3] + 3 * offset, &part->this, sizeb);
			send_sizes[3] += 1;
			// mcord->x > border_right == true
		}


		next_part = part->next;
		free(part);
		part = next_part;
	}

	// The list is now empty, re initialize
	init(&parts_to_send);


	for(int i = 0; i < 4; ++i){
		if(dest[i] == -1){
			continue;
		}

		MPI_Isend(send_buff, send_sizes[i] * sizeof(particle_t), MPI_BYTE, dest[i],
			iteration, MPI_COMM_WORLD, mpi_rq + i);
	}

	for(int i = 0; i < 4; ++i){
		if(dest[i] == -1){
			continue;
		}

		MPI_Recv(rec_buff + i * offset, offset * sizeof(particle_t), MPI_BYTE, dest[i],
				iteration, MPI_COMM_WORLD, status + i);
	}


	int rec_count = 0;
	for(int i = 0; i < 4; ++i){
		if(dest[i] == -1){
			continue;
		}

		MPI_Get_count(status + i, MPI_BYTE, &rec_count);
		rec_count /= sizeof(particle_t);

		// TODO remove this once Im sure that it works
		if(rec_count != 0){
			printf("%i received %i\n", rank, rec_count);
			psize(&particles, rank);
		}

		for(int j = 0; j < rec_count; ++j){
			particle_t* rec_part = rec_buff + j + (offset * i);
			pcord_t* rec_cord = &(rec_part->pcord);

			append(&particles, create_particle(rec_cord->x, rec_cord->y,
				 	rec_cord->vx, rec_cord->y));

		}

		// TODO remove this once Im sure that it works
		if(rec_count != 0){
			psize(&particles, rank);
		}
	}

	// TODO remove this
	printf("End of iteration %i\n", iteration);
	} // End of the iteration

	/*
	MPI_Reduce(&my_pressure, &global_pressure, 1, MPI_DOUBLE,
		 	MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		printf("Average pressure = %f\n", global_pressure / (WALL_LENGTH*time_max));
	}
*/
	// 3 - Loop for all iterations, need barrier
	// 4 - Collect sum of momentum at the end of each iteration


	MPI_Finalize();
	return 0;

}
