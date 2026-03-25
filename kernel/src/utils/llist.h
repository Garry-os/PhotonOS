#pragma once
#include <stddef.h>

typedef struct LLHeader
{
	struct LLHeader* next;

	// ...
} LLHeader;

void* LL_Allocate(void** first, size_t size);
void LL_Add(void** first, void* ptr);

void LL_Delete(void** first, void* ptr);

