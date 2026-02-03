#include "vmm.h"
#include <stdint.h>
#include <stddef.h>
#include <paging.h>
#include <qemu/print.h>
#include <x86_64/cpu.h>
#include <boot.h>
#include <pmm.h>

uint64_t* g_PageDir = NULL;

void InitVMM()
{
	// Get limine's page table
	uint64_t pdPhys = 0;
	asm volatile("movq %%cr3,%0" : "=r"(pdPhys));

	if (!pdPhys)
	{
		dbg_printf("[VMM] Can't get limine's page directory from CR3!\n");
		panic();
	}

	uint64_t pdVirt = pdPhys + g_BootInfo.hhdmOffset;
	g_PageDir = (uint64_t*)pdVirt;
}

void vmm_MapPage(void* virt, void* phys, uint64_t flags)
{
	pt_MapPage(g_PageDir, (uint64_t)virt, (uint64_t)phys, flags);
}

void* vmm_Allocate(size_t pages)
{
	void* phys = pmm_Allocate(pages);

	// Convert to virtual address
	uint64_t addr = (uint64_t)phys + g_BootInfo.hhdmOffset;

	return (void*)addr;
}

void vmm_Free(void* address, size_t pages)
{
	// TODO
	(void)address;
	(void)pages;
}

void* vmm_VirtToPhys(void* virt)
{
	return (void*)pt_VirtToPhys(g_PageDir, (uint64_t)virt);
}

