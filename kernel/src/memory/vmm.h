#pragma once
#include <stdint.h>

void InitVMM();
void vmm_MapPage(void* virt, void* phys, uint64_t flags);

