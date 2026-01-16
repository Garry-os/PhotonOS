//
// OsLoader's boot protocol
//
#pragma once
#include <stdint.h>
#include <stddef.h>

#define BOOT_MAGIC 0x1122334455

typedef struct
{
	void* base; // GOP framebuffer base address
	size_t fbSize; // Framebuffer size
	uint32_t width;
	uint32_t height;
	uint32_t pixelsPerScanline;
} Fb_Info;

typedef struct
{
	uint32_t magic; // Boot info magic code
	uint64_t kernelPhysicalAddress;

	// Framebuffer info
	Fb_Info fbInfo;
} BootInfo;


