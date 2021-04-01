#include "convolution.h"
#include "fft.h"
#include "ift.h"

void complexMultiplication(float complex* sample, float complex* ir, float complex* result, int size)
{
    for(int i = 0; i < size; ++i)
        *result++ = *sample++ * *ir++;
}

void convolve(float complex* sample, float complex* ir, float complex* result, int size)
{
	fft(sample, size);
	fft(ir, size);
    complexMultiplication(sample, ir, result, size);
    ift(result, size);
}
