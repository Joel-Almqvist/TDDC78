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

	uint threshfilter_sum(const int xsize, const int ysize, pixel* src){
		uint i, psum, nump, sum;
		nump = xsize * ysize;

		for (i = 0, sum = 0; i < nump; i++)
		{
			sum += (uint)src[i].r + (uint)src[i].g + (uint)src[i].b;
		}
		return sum;
	};

	void threshfilter_exec(const int xsize, const int ysize, pixel* src, unsigned int sum){
		uint i, psum, nump;
		nump = xsize * ysize;

		//uint sum = tot_sum / nump;

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
	};
