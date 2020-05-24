#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>


#include "coordinate.h"
#include "definitions.h"
#include "physics.h"


#define INIT_NO_PARTICLES 2000    /* Initial number of particles/processor */
#define BOX_HORIZ_SIZE 10000.0
#define BOX_VERT_SIZE 10000.0

//Feel free to change this program to facilitate parallelization.

float rand1(){
	return (float)( rand()/(float) RAND_MAX );
}

void init_collisions(bool *collisions, unsigned int max){
	for(unsigned int i=0;i<max;++i)
		collisions[i]=0;
}


// Thank you for this function August
double timediff(struct timespec *begin, struct timespec *end)
{
	double sec = 0.0, nsec = 0.0;
	if ((end->tv_nsec - begin->tv_nsec) < 0)
	{
		sec  = (double)(end->tv_sec  - begin->tv_sec  - 1);
		nsec = (double)(end->tv_nsec - begin->tv_nsec + 1000000000);
	} else
	{
		sec  = (double)(end->tv_sec  - begin->tv_sec );
		nsec = (double)(end->tv_nsec - begin->tv_nsec);
	}
	return sec + nsec / 1E9;
}


int main(int argc, char** argv){


	unsigned int time_stamp = 0, time_max;
	float pressure = 0;
	struct timespec starttime;
	struct timespec endtime;

	// parse arguments
	if(argc != 2) {
		fprintf(stderr, "Usage: %s simulation_time\n", argv[0]);
		fprintf(stderr, "For example: %s 10\n", argv[0]);
		exit(1);
	}
	clock_gettime(CLOCK_MONOTONIC, &starttime);
	time_max = atoi(argv[1]);


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

	clock_gettime(CLOCK_MONOTONIC, &endtime);
	printf("Execution time: %f\n", timediff(&starttime, &endtime));

	free(particles);
	free(collisions);

	return 0;

}
