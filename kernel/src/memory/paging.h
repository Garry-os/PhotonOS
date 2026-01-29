#pragma once
#include <stdint.h>
#include <stddef.h>

// Paging flags
#define PF_PRESENT (1 << 0)
#define PF_RW (1 << 1)
#define PF_USER (1 << 2)
#define PF_PWT (1 << 3)
#define PF_PCD (1 << 4)
#define PF_ACCESS (1 << 5)
#define PF_DIRTY (1 << 6)
#define PF_PS (1 << 7)
#define PF_PAT (1 << 7)
#define PF_GLOBAL (1 << 8)
#define PF_SHARED (1 << 9)

/*
	Page entry indexing macros
*/
#define PGSHIFT_PML4E 39
#define PGSHIFT_PDPTE 30
#define PGSHIFT_PDE 21
#define PGSHIFT_PTE 12
#define PGMASK_ENTRY 0x1ff
#define PGMASK_OFFSET 0x3ff

#define PML4E(a) (((a) >> PGSHIFT_PML4E) & PGMASK_ENTRY)
#define PDPTE(a) (((a) >> PGSHIFT_PDPTE) & PGMASK_ENTRY)
#define PDE(a) (((a) >> PGSHIFT_PDE) & PGMASK_ENTRY)
#define PTE(a) (((a) >> PGSHIFT_PTE) & PGMASK_ENTRY)

// Strip upper 16 bits
#define X86_64_STRIP16(a) ((uintptr_t)(a) & 0xFFFFFFFFFFFF)

// Get page table entry address macro
// The way this work is that it will zeros out the "flags" bits
// Leaving us with only the 48 bit address in the middle
#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)

#define P_PHYS_ADDR(x) ((x) & ~0xFFF)

void pt_MapPage(uint64_t* pml4, uint64_t virtAddr, uint64_t physAddr, uint64_t flags);

