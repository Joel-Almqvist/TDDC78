/*
  File: blurfilter.h
  Declaration of pixel structure and blurfilter function.
 */

#ifndef _BLURFILTER_H_
#define _BLURFILTER_H_


/* NOTE: This structure must not be padded! */
typedef struct _pixel {
	unsigned char r,g,b;
} pixel;


	typedef struct _blur_args {
		int xsize, ysize, radius, min_y, max_y;
		double* w;
		pixel* src;
		pixel* buffer;
		pthread_barrier_t* barrier;
	} blur_args;


	void* blurfilter_pt(void* args);

#endif
