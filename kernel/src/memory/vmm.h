#pragma once
#include <stdint.h>
#include <stddef.h>

void InitVMM();
void vmm_MapPage(void* virt, void* phys, uint64_t flags);

void* vmm_Allocate(size_t pages);
void vmm_Free(void* address, size_t pages);

void* vmm_VirtToPhys(void* virt);

