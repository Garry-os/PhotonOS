#pragma once
#include <stddef.h>

// 4 KB
#define PAGE_SIZE 0x1000

void InitPMM();
void* pmm_AllocatePages(size_t pages);

void FreePages(void* address, size_t pages);

