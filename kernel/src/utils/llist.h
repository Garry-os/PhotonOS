#pragma once
#include <stddef.h>

typedef struct LLHeader
{
	struct LLHeader* next;

	// ...
} LLHeader;

void* LL_Allocate(void** first, size_t size);

