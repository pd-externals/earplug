#include <stdio.h>
#include "convolution.h"

int main()
{
	float complex dirac1[128] = {0.0f};
	float complex dirac2[128] = {0.0f};
	float complex result[128] = {0.0f};

	dirac1[2] = 0.8f;
	dirac2[2] = 0.5f;

	convolve(dirac1, dirac2, result, 128);

	for(int i = 0; i < 10; i++)
	{
		printf("index:%d value:%f\n", i, creal(result[i]));
	}

	return 0;
}