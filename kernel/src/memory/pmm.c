#include "pmm.h"
#include <utils/bitmap.h>
#include <limine.h>
#include <qemu/print.h>
#include <x86_64/cpu.h>
#include <utils/memory.h>
#include <boot.h>

bitmap_t g_Bitmap;

void LockPages(void* address, size_t pages);

void InitPMM()
{
	// Calculate the total memory and bitmap buffer size
	size_t bitmapSize = g_BootInfo.mmTotal / PAGE_SIZE / 8 + 1;
	g_Bitmap.size = bitmapSize;

	void* bitmapPtr = NULL;

	// Find a memory region that fits the bitmap's buffer
	for (int i = 0; i < g_BootInfo.mmNumEntries; i++)
	{
		struct limine_memmap_entry* entry = g_BootInfo.mmEntries[i];
		if (entry->type == LIMINE_MEMMAP_USABLE)
		{
			// Check the size
			if (entry->length >= bitmapSize)
			{
				bitmapPtr = (void*)entry->base;
				break;
			}
		}
	}

	if (!bitmapPtr)
	{
		// Not enough memory
		dbg_printf("[PMM] Not enough memory for bitmap!\n");
		panic();
	}

	g_Bitmap.buffer = (uint8_t*)bitmapPtr + g_BootInfo.hhdmOffset;

	// Loop through the entire memory map again
	// to mark the page if it is usable or not
	memset(g_Bitmap.buffer, 0xFF, bitmapSize);
	for (int i = 0; i < g_BootInfo.mmNumEntries; i++)
	{
		struct limine_memmap_entry* entry = g_BootInfo.mmEntries[i];

		if (entry->type == LIMINE_MEMMAP_USABLE)
		{
			FreePages((void*)entry->base, entry->length / PAGE_SIZE);
		}
	}

	// Lock important pages
	LockPages((void*)0, 0x10); // First 16 pages
	LockPages((void*)bitmapPtr, bitmapSize); // Bitmap
}

// Return an avaliable address
void* pmm_AllocatePage()
{
	for (uint64_t i = 0; i < g_Bitmap.size * 8; i++)
	{
		if (BitmapGet(&g_Bitmap, i))
		{
			continue;
		}

		void* address = (void*)(i * PAGE_SIZE);
		LockPages(address, 1);
		return address;
	}

	// Out of memory
	dbg_printf("[PMM] Out of memory!\n");
	panic();

	return NULL;
}

void LockPage(void* address)
{
	uint64_t index = (uint64_t)address / PAGE_SIZE;
	BitmapSet(&g_Bitmap, index, true);
}

void LockPages(void* address, size_t pages)
{
	for (size_t i = 0; i < pages; i++)
	{
		LockPage((void*)((uint64_t)address + (i * PAGE_SIZE)));
	}
}

void FreePage(void* address)
{
	uint64_t index = (uint64_t)address / PAGE_SIZE;
	BitmapSet(&g_Bitmap, index, false);
}

void FreePages(void* address, size_t pages)
{
	for (size_t i = 0; i < pages; i++)
	{
		FreePage((void*)((uint64_t)address + (i * PAGE_SIZE)));
	}
}

