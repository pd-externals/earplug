#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "convolution.h"

void assert_eq(float expected, float actual)
{
	float dif = fabsf(expected -actual);
	assert(dif < 0.001f);
}

int main()
{
	float sample64[64] = {0.0f};
	float sample128[128] = {0.0f};
	float sample512[512] = {0.0f};
	float sample2048[2048] = {0.0f};

	float result64[64] = {0.0f};
	float result128[128] = {0.0f};
	float result512[512] = {0.0f};
	float result2048[2048] = {0.0f};

	float ir[128] = {0.0f};

	// the second last sample have dirac
	sample64[62] = 1.f;
	sample128[126] = 1.f;
	sample512[510] = 1.f;
	sample2048[2046] = 1.f;

	ir[1] = 0.3f; // generate one sample delay + attenuation

	//expect convolution result in buffers with one inpulse (amp = 0.3) at the end of the buffer

	convolve(sample64, 64, ir, result64);
	for(int i = 0; i < 63; ++i)
		assert_eq(result64[i], 0.0f);
	assert_eq(result64[63], 0.3f);
	puts("block size 64: OK");

	convolve(sample128, 128, ir, result128);
	for(int i = 0; i < 127; ++i)
		assert_eq(result128[i], 0.0f);
	assert_eq(result128[127], 0.3f);
	puts("block size 128: OK");

	convolve(sample512, 512, ir, result512);
	for(int i = 0; i < 511; ++i)
		assert_eq(result512[i], 0.0f);
	assert_eq(result512[511], 0.3f);
	puts("block size 512: OK");

	convolve(sample2048, 2048, ir, result2048);
	for(int i = 0; i < 2047; ++i)
		assert_eq(result2048[i], 0.0f);
	assert_eq(result2048[2047], 0.3f);
	puts("block size 2048: OK");

	return 0;
}