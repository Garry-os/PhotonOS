#include "boot.h"
#include <qemu/print.h>
#include <x86_64/cpu.h>

bootInfo_t g_BootInfo;

// Request 4 levels paging
static volatile struct limine_paging_mode_request liminePagingReq = {
	.id = LIMINE_PAGING_MODE_REQUEST_ID,
	.revision = 0,
	.mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

// Request hhdm
static volatile struct limine_hhdm_request limineHHDMReq = {
	.id = LIMINE_HHDM_REQUEST_ID,
	.revision = 0
};

// Request memory map
static volatile struct limine_memmap_request limineMMReq = {
	.id = LIMINE_MEMMAP_REQUEST_ID,
	.revision = 0
};

void InitBootInfo()
{
	struct limine_paging_mode_response* liminePagingRes = liminePagingReq.response;
	
	// Check if limine actually gave us 4 levels page table
	if (liminePagingRes->mode != LIMINE_PAGING_MODE_X86_64_4LVL)
	{
		dbg_printf("[Kernel] 4 levels page table isn't avaliable\n");
		panic();
	}

	struct limine_hhdm_response* limineHHDMRes = limineHHDMReq.response;
	g_BootInfo.hhdmOffset = limineHHDMRes->offset;

	struct limine_memmap_response* limineMMRes = limineMMReq.response;
	g_BootInfo.mmEntries = limineMMRes->entries;
	g_BootInfo.mmNumEntries = limineMMRes->entry_count;

	// Calculate total amount of memory
	g_BootInfo.mmTotal = 0;

	for (uint64_t i = 0; i < g_BootInfo.mmNumEntries; i++)
	{
		struct limine_memmap_entry* entry = limineMMRes->entries[i];
		if (entry->type != LIMINE_MEMMAP_RESERVED)
		{
			g_BootInfo.mmTotal += entry->length;
		}
	}
}

