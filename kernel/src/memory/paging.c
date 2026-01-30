//
// Page table manager for x86_64
//
#include "paging.h"
#include <pmm.h>
#include <utils/memory.h>
#include <boot.h>
#include <x86_64/cpu.h>

// Allocate page table
uint64_t pt_Allocate()
{
	uint64_t phys = (uint64_t)pmm_Allocate(1);
	
	void* virt = (void*)(phys + g_BootInfo.hhdmOffset);
	memset(virt, 0, PAGE_SIZE);

	return phys;
}

void pt_MapPage(uint64_t* pml4, uint64_t virtAddr, uint64_t physAddr, uint64_t flags)
{
	// Strip out the upper 16 bit of the virtual address
	virtAddr = X86_64_STRIP16(virtAddr);

	// Get the page tables indexing
	uint64_t pml4_i = PML4E(virtAddr);
    uint64_t pdp_i = PDPTE(virtAddr);
    uint64_t pd_i = PDE(virtAddr);
    uint64_t pt_i = PTE(virtAddr);

	// PML4 entry
	if (!(pml4[pml4_i] & PF_PRESENT))
	{
		uint64_t target = pt_Allocate();
		pml4[pml4_i] = target | PF_PRESENT | PF_RW;
	}

	// PDP entry
	uint64_t* pdp = (uint64_t*)(PTE_GET_ADDR(pml4[pml4_i]) + g_BootInfo.hhdmOffset);
	
	if (!(pdp[pdp_i] & PF_PRESENT))
	{
		uint64_t target = pt_Allocate();
		pdp[pdp_i] = target | PF_PRESENT | PF_RW;
	}

	// PD entry
	uint64_t* pd = (uint64_t*)(PTE_GET_ADDR(pdp[pdp_i]) + g_BootInfo.hhdmOffset);

	if (!(pd[pd_i] & PF_PRESENT))
	{
		uint64_t target = pt_Allocate();
		pd[pd_i] = target | PF_PRESENT | PF_RW;
	}

	// PT entry
	uint64_t* pt = (uint64_t*)(PTE_GET_ADDR(pd[pd_i]) + g_BootInfo.hhdmOffset);

	pt[pt_i] = (P_PHYS_ADDR(physAddr)) | PF_PRESENT | PF_RW | flags;
	invalidate(virtAddr);
}

