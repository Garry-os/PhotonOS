#pragma once
#include <stddef.h>

void InitHeap();

void* malloc(size_t size);
void free(void* address);

