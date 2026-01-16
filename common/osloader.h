//
// OsLoader's boot protocol
//
#pragma once
#include <stdint.h>

#define BOOT_MAGIC 0x1122334455

typedef struct
{
	uint32_t magic; // Boot info magic code
	uint64_t kernelPhysicalAddress;
} BootInfo;


