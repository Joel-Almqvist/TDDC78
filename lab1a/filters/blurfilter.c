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

// xsize and ysize are the dimensions of the image
void blurfilter(const int xsize, const int ysize, pixel* src,
	const int radius, const double *w, const int min_y, const int max_y)
{
	int x, y, x2, y2, wi;
	double r, g, b, n, wc;

	pixel *dst = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);

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
			pix(dst,x,y, xsize)->r = r/n;
			pix(dst,x,y, xsize)->g = g/n;
			pix(dst,x,y, xsize)->b = b/n;
		}
	}

	for (y=min_y; y<max_y; y++)
	{
		for (x=0; x<xsize; x++)
		{
			r = w[0] * pix(dst, x, y, xsize)->r;
			g = w[0] * pix(dst, x, y, xsize)->g;
			b = w[0] * pix(dst, x, y, xsize)->b;
			n = w[0];
			for ( wi=1; wi <= radius; wi++)
			{
				wc = w[wi];
				y2 = y - wi;
				if (y2 >= 0)
				{
					r += wc * pix(dst, x, y2, xsize)->r;
					g += wc * pix(dst, x, y2, xsize)->g;
					b += wc * pix(dst, x, y2, xsize)->b;
					n += wc;
				}
				y2 = y + wi;
				if (y2 < ysize)
				{
					r += wc * pix(dst, x, y2, xsize)->r;
					g += wc * pix(dst, x, y2, xsize)->g;
					b += wc * pix(dst, x, y2, xsize)->b;
					n += wc;
				}
			}
			pix(src,x,y, xsize)->r = r/n;
			pix(src,x,y, xsize)->g = g/n;
			pix(src,x,y, xsize)->b = b/n;
		}
	}
	free(dst);
}




// void blurfilter_pt(const int xsize, const int ysize, pixel* src,
// 	const int radius, const double *w, const int min_y,
// 	const int max_y, pixel* dst)
void* blurfilter_pt(void* args_void)
{
	blur_args* args = (blur_args*) args_void;
	const int xsize = args->xsize;
	const int ysize = args->ysize;
	const int radius = args->radius;
	const int min_y = args->min_y;
	const int max_y = args->max_y;
	const double* w = args->w;
	pixel* src = args->src;
	pixel* dst = args->dst;
	pthread_rwlock_t* rw_lock = args->rw_ptr;

	printf("xsize = %i  ysize = %i  radius = %i  min_y = %i  max_y = %i\n",
	xsize, ysize, radius, min_y, max_y);

	int x, y, x2, y2, wi;
	double r, g, b, n, wc;
	//pixel *dst = (pixel*) malloc(sizeof(pixel) * MAX_PIXELS);

//	pixel* row = (pixel*) malloc(sizeof(pixel) * xsize);

	// TODO Split this up into 2 functions, one which does the changes row-wise
	// and one which does it column wise.

	// Row thing is super easy, for clolumns handle multiple values at the same time
	// to make the cache happy. Also remember to use a barrier

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
			pthread_rwlock_wrlock(rw_lock);
			pix(dst,x,y, xsize)->r = r/n;
			pix(dst,x,y, xsize)->g = g/n;
			pix(dst,x,y, xsize)->b = b/n;
			pthread_rwlock_unlock(rw_lock);
			/*
			pix(row,x,0, xsize)->r = r/n;
			pix(row,x,0, xsize)->g = g/n;
			pix(row,x,0, xsize)->b = b/n;
			*/
		}

	}

	for (y=min_y; y<max_y; y++)
	{
		for (x=0; x<xsize; x++)
		{
			pthread_rwlock_rdlock(rw_lock);
			r = w[0] * pix(dst, x, y, xsize)->r;
			g = w[0] * pix(dst, x, y, xsize)->g;
			b = w[0] * pix(dst, x, y, xsize)->b;
			pthread_rwlock_unlock(rw_lock);
			n = w[0];
			for ( wi=1; wi <= radius; wi++)
			{
				wc = w[wi];
				y2 = y - wi;
				if (y2 >= 0)
				{
					pthread_rwlock_rdlock(rw_lock);
					r += wc * pix(dst, x, y2, xsize)->r;
					g += wc * pix(dst, x, y2, xsize)->g;
					b += wc * pix(dst, x, y2, xsize)->b;
					pthread_rwlock_unlock(rw_lock);
					n += wc;
				}
				y2 = y + wi;
				if (y2 < ysize)
				{
					pthread_rwlock_rdlock(rw_lock);
					r += wc * pix(dst, x, y2, xsize)->r;
					g += wc * pix(dst, x, y2, xsize)->g;
					b += wc * pix(dst, x, y2, xsize)->b;
					pthread_rwlock_unlock(rw_lock);
					n += wc;
				}
			}
			pthread_rwlock_wrlock(rw_lock);
			pix(dst,x,y, xsize)->r = r/n;
			pix(dst,x,y, xsize)->g = g/n;
			pix(dst,x,y, xsize)->b = b/n;
			pthread_rwlock_unlock(rw_lock);
		}
	}
	return NULL;
	//free(dst);
}
