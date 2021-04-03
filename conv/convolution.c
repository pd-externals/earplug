#include "convolution.h"
#include "fft.h"
#include "ift.h"

void copyToComplex(float* buffer, int size, float complex* complexBuffer)
{
	for(int i = 0; i < size; ++i)
		complexBuffer[i] = buffer[i];
}

void copyFromComplex(float complex* complexBuffer, int size, float* buffer)
{
	for(int i = 0; i < size; ++i)
		buffer[i] = creal(complexBuffer[i]);
}

void convolve(float* sample, int size, float* ir, float* result)
{
	// fft ir filter
	float complex complexIr[128] = {0.f};
	copyToComplex(ir, 128, complexIr);
	fft(complexIr, 128);

	float complex complexResult[128] = {0.f};
	int remainingSamples = size;

	do
	{
		float complex complexSample[128] = {0.f};
		int samplesToBeCopied = remainingSamples > 128 ? 128 : remainingSamples; 
		copyToComplex(sample, samplesToBeCopied, complexSample);
		fft(complexSample, 128);

		for(int i = 0; i < 128; ++i)
    	    complexResult[i] = complexSample[i] * complexIr[i];

	    ift(complexResult, 128);
    	copyFromComplex(complexResult, samplesToBeCopied, result);

		remainingSamples -= 128;
		sample += 128;
		result += 128;
	}while(remainingSamples > 0);

}
