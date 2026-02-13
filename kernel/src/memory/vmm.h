#pragma once
#include <stdint.h>
#include <stddef.h>

void InitVMM();
void vmm_MapPage(void* virt, void* phys, uint64_t flags);

void* vmm_Allocate(size_t pages);
void vmm_Free(void* address, size_t pages);

void* vmm_VirtToPhys(void* virt);

void vmm_SwitchPdUnsafe(uint64_t* pd);
void vmm_SwitchPd(uint64_t* pd);
void vmm_SwitchPdGlobal(uint64_t* pd);

uint64_t* vmm_CopyKernelPd();

uint64_t* vmm_GetCurrentPd();

