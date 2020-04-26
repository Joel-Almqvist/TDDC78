/*
  File: blurfilter.c
  Implementation of blurfilter function.
 */

#include <stdio.h>
#include <stdlib.h>
#include "blurfilter.h"
#include "ppmio.h"
#include <pthread.h>

pixel* pix(pixel* image, const int xx, const int yy, const int xsize)
{
	int off = xsize*yy + xx;
	return (image + off);
}


void* blurfilter_pt(void* args_void) {
	blur_args* args = (blur_args*) args_void;
	const int xsize = args->xsize;
	const int ysize = args->ysize;
	const int radius = args->radius;
	const int min_y = args->min_y;
	const int max_y = args->max_y;
	const double* w = args->w;
	pthread_barrier_t* barrier = args->barrier;
	pixel* src = args->src;
	pixel* buff = args->buffer;

	//printf("xsize = %i  ysize = %i  radius = %i  min_y = %i  max_y = %i\n",
	//xsize, ysize, radius, min_y, max_y);

	int x, y, x2, y2, wi;
	double r, g, b, n, wc;

	for (y=min_y; y<max_y; y++)
	{
		for (x=0; x<xsize; x++)
		{
			r = w[0] * pix(src, x, y, xsize)->r;
			g = w[0] * pix(src, x, y, xsize)->g;
			b = w[0] * pix(src, x, y, xsize)->b;
			n = w[0];
			for ( wi=1; wi <= radius; wi++)
			{
				wc = w[wi];
				x2 = x - wi;
				if (x2 >= 0)
				{
					r += wc * pix(src, x2, y, xsize)->r;
					g += wc * pix(src, x2, y, xsize)->g;
					b += wc * pix(src, x2, y, xsize)->b;
					n += wc;
				}
				x2 = x + wi;
				if (x2 < xsize)
				{
					r += wc * pix(src, x2, y, xsize)->r;
					g += wc * pix(src, x2, y, xsize)->g;
					b += wc * pix(src, x2, y, xsize)->b;
					n += wc;
				}
			}
			pix(buff,x,y, xsize)->r = r/n;
			pix(buff,x,y, xsize)->g = g/n;
			pix(buff,x,y, xsize)->b = b/n;
		}
	}

	pthread_barrier_wait(barrier);


	for (y=min_y; y<max_y; y++)
	{
		for (x=0; x<xsize; x++)
		{
			r = w[0] * pix(buff, x, y, xsize)->r;
			g = w[0] * pix(buff, x, y, xsize)->g;
			b = w[0] * pix(buff, x, y, xsize)->b;
			n = w[0];
			for ( wi=1; wi <= radius; wi++)
			{
				wc = w[wi];
				y2 = y - wi;
				if (y2 >= 0)
				{
					r += wc * pix(buff, x, y2, xsize)->r;
					g += wc * pix(buff, x, y2, xsize)->g;
					b += wc * pix(buff, x, y2, xsize)->b;
					n += wc;
				}
				y2 = y + wi;
				if (y2 < ysize)
				{
					r += wc * pix(buff, x, y2, xsize)->r;
					g += wc * pix(buff, x, y2, xsize)->g;
					b += wc * pix(buff, x, y2, xsize)->b;
					n += wc;

				}
			}
			pix(src, x,y, xsize)->r = r/n;
			pix(src,x,y, xsize)->g = g/n;
			pix(src,x,y, xsize)->b = b/n;
		}
	}
	return NULL;
}
