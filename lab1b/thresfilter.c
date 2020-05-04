#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "thresfilter.h"

#define uint unsigned int

void thresfilter(const int xsize, const int ysize, pixel* src)
{
	uint sum, i, psum, nump;
	nump = xsize * ysize;

	for (i = 0, sum = 0; i < nump; i++)
	{
		sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
	}

	sum /= nump;

	for (i = 0; i < nump; i++)
	{
		psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
		if (sum > psum)
		{
			src[i].r = src[i].g = src[i].b = 0;
		}
		else
		{
			src[i].r = src[i].g = src[i].b = 255;
		}
	}
}

	void* threshfilter_sum(void* inc_args){
		thresh_sum_args* args = (thresh_sum_args*) inc_args;
		int xsize = args->xsize;
		int y_min = args->y_min;
		int y_max = args->y_max;
		pixel* src = args->src;
		uint* return_val = args->return_val;

		uint sum = 0;
		int max_size = y_max * xsize;

		uint i;
		for ( i = y_min * xsize;  i < max_size; i++)
		{
			sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
		}
		 *return_val = sum;
		return NULL;
	};

	void* threshfilter_exec(void* inc_args){
		thresh_exec_args* args = (thresh_exec_args*) inc_args;
		int xsize = args->xsize;
		int y_min = args->y_min;
		int y_max = args->y_max;
		pixel* src = args->src;
		uint avg = args->avg;

		uint i, psum, nump;
		nump = y_max * xsize;
		uint end = y_max*xsize;


		for (i = (uint) y_min*xsize; i < end; i++)
		{
			psum = (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
			if (avg > psum)
			{
				src[i].r = src[i].g = src[i].b = 0;
			}
			else
			{
				src[i].r = src[i].g = src[i].b = 255;
			}
		}
		return NULL;
	};
