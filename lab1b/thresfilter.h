/*
  File: thresfilter.h
  Declaration of pixel structure and thresfilter function.
 */

#ifndef _THRESFILTER_H_
#define _THRESFILTER_H_

/* NOTE: This structure must not be padded! */
typedef struct _pixel {
	unsigned char r,g,b;
} pixel;


typedef struct _thresh_sum_args {
	int xsize, y_min, y_max;
	pixel* src;
	uint* return_val;
} thresh_sum_args;


typedef struct _thresh_exec_args {
	int xsize, y_min, y_max;
	pixel* src;
	uint avg;
} thresh_exec_args;


void thresfilter(const int xsize, const int ysize, pixel* src);

void* threshfilter_exec(void* args);

void* threshfilter_sum(void* args);

#endif
