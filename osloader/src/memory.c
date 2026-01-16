#include "memory.h"
#include <stdint.h>

int memcmp(const void* a, const void* b, size_t num)
{
	const uint8_t* u8A = (uint8_t*)a;
	const uint8_t* u8B = (uint8_t*)b;

	for (size_t i = 0; i < num; i++)
	{
		if (u8A[i] != u8B[i])
		{
			return u8A[i] - u8B[i];
		}
	}

	return 0;
}

