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

void blurfilter(const int xsize, const int ysize, pixel* src,
	const int radius, const double *w, const int min_y, const int max_y);


	typedef struct _blur_args {
		int xsize, ysize, radius, min, max;
		double* w;
		pixel* src;
		//pthread_rwlock_t* rw_ptr;
	} blur_args;


	// void blurfilter_pt(const int xsize, const int ysize, pixel* src,
	// 	const int radius, const double *w, const int min_y,
	// 	const int max_y, pixel* dst);

	void* blurfilter_pt(void* args);

#endif
