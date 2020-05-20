#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

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
		double* border_bot, double* border_left){

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
			int my_col = (rank) % nr_cols;
			int my_row = (rank + 1) / nr_rows;

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

			free(den);
		}

/* Searches the list for collisions, removes any colliding elements
 * and adds them to the collision list.
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



int main(int argc, char** argv){



	unsigned int time_stamp = 0, time_max;
	float pressure = 0;

	// parse arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: %s simulation_time\n", argv[0]);
		fprintf(stderr, "For example: %s 10\n", argv[0]);
		exit(1);
	}

	time_max = atoi(argv[1]);




	int nr_proc;   // number of started MPI processes
	int rank;  // my rank
	MPI_Status status;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &nr_proc );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	// TODO change this
	nr_proc = 6;
	rank = 4;

	int nr_rows;
	int nr_cols;

	double border_right, border_left, border_top, border_bot;

	set_box_limits(&nr_rows, &nr_cols, nr_proc, rank, &border_top,
		&border_right, &border_bot, &border_left);




	particle_list particles;
	particle_list collisions;

	int my_start_partition;
	if(rank == nr_proc - 1){
		my_start_partition = INIT_NO_PARTICLES / nr_proc + INIT_NO_PARTICLES % nr_proc;
	}
	else{
		my_start_partition = INIT_NO_PARTICLES / nr_proc;
	}



	init(&particles);

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

	plist_elem* part = particles.first;

	while(true){
		part = search_collision(&particles, part,  &collisions);

		if(part == NULL){
			break;
		}
	}


	// 1 - Interact all consecutive elements in collision

	// 2 - Move all elements in particles

	// 3 - Send all which move out of bounds

	// 4 - Repeat for all iterations






	return 0;

}
