#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include <mpi.h>

#include "coordinate.h"
#include "definitions.h"
#include "physics.h"

//Feel free to change this program to facilitate parallelization.

float rand1(){
	return (float)( rand()/(float) RAND_MAX );
}

void init_collisions(bool *collisions, unsigned int max){
	for(unsigned int i=0;i<max;++i)
		collisions[i]=0;
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
	// The border are inclusive
	double border_right, border_left, border_top, border_bot;

	set_box_limits(&nr_rows, &nr_cols, nr_proc, rank, &border_top,
		&border_right, &border_bot, &border_left);


	exit(0);


	/* Initialize */
	// 1. set the walls
	cord_t wall;
	wall.y0 = wall.x0 = 0;
	wall.x1 = BOX_HORIZ_SIZE;
	wall.y1 = BOX_VERT_SIZE;


	// 2. allocate particle bufer and initialize the particles
	pcord_t *particles = (pcord_t*) malloc(INIT_NO_PARTICLES*sizeof(pcord_t));
	bool *collisions=(bool *)malloc(INIT_NO_PARTICLES*sizeof(bool) );

	srand( time(NULL) + 1234 );

	float r, a;
	for(int i=0; i<INIT_NO_PARTICLES; i++){
		// initialize random position
		particles[i].x = wall.x0 + rand1()*BOX_HORIZ_SIZE;
		particles[i].y = wall.y0 + rand1()*BOX_VERT_SIZE;

		// initialize random velocity
		r = rand1()*MAX_INITIAL_VELOCITY;
		a = rand1()*2*PI;
		particles[i].vx = r*cos(a);
		particles[i].vy = r*sin(a);
	}


	unsigned int p, pp;

	/* Main loop */
	for (time_stamp=0; time_stamp<time_max; time_stamp++) { // for each time stamp

		init_collisions(collisions, INIT_NO_PARTICLES);

		for(p=0; p<INIT_NO_PARTICLES; p++) { // for all particles
			if(collisions[p]) continue;

			/* check for collisions */
			for(pp=p+1; pp<INIT_NO_PARTICLES; pp++){
				if(collisions[pp]) continue;
				float t=collide(&particles[p], &particles[pp]);
				if(t!=-1){ // collision
					collisions[p]=collisions[pp]=1;
					interact(&particles[p], &particles[pp], t);
					break; // only check collision of two particles
				}
			}

		}

		// move particles that has not collided with another
		for(p=0; p<INIT_NO_PARTICLES; p++)
			if(!collisions[p]){
				feuler(&particles[p], 1);

				/* check for wall interaction and add the momentum */
				pressure += wall_collide(&particles[p], wall);
			}


	}


	printf("Average pressure = %f\n", pressure / (WALL_LENGTH*time_max));

	free(particles);
	free(collisions);

	return 0;

}
