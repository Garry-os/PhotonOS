#include "stack.h"
#include <malloc.h>

#define STACK_SIZE 4096 // 4 KB

void* AllocateStack()
{
	void* bottom = malloc(STACK_SIZE);
	void* top = (void*)((size_t)bottom + STACK_SIZE);

	return top;
}

