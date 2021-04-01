#include "convolution.h"
#include "fft.h"
#include "ift.h"

void complexMultiplication(float complex* sample, float complex* ir, int size)
{
    while(size--)
        *sample++ *= *ir++;
}

void convolve(float complex* sample, float complex* ir, int size)
{
	fft(sample, size);
	fft(ir, size);
    complexMultiplication(sample, ir, size);
    ift(sample, size);
}

void clean(float complex* buffer, int size)
{
	while(size--)
		*buffer++ = 0.f + 0.fI;
}