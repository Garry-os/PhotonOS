#pragma once
#include <stdint.h>
#include <limine.h>

typedef struct
{
	uint64_t hhdmOffset; // Higher half direct map offset

	uint64_t mmTotal; // Total amout of memory
	uint64_t mmNumEntries; // Total number of memory map entries
	LIMINE_PTR(struct limine_memmap_entry**) mmEntries;
} bootInfo_t;

extern bootInfo_t g_BootInfo;

void InitBootInfo();

