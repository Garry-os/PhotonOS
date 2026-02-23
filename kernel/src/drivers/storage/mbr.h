#pragma once
#include <stdint.h>
#include <storage/block.h>
#include <stdbool.h>

// MBR partition entry
typedef struct
{
	uint8_t attribute; // bit 7 set = active or bootable
	uint8_t chsAddressStart[3]; // Partition start (CHS)
	uint8_t type;
	uint8_t chsAddressEnd[3]; // Partition end (CHS)
	uint32_t lbaStart;
	uint32_t sectorsNum;
} __attribute__((packed)) MBR_Entry;

typedef struct
{
	uint8_t bootStrap[440];
	uint32_t signature; // Optional
	uint16_t _reserved;
	MBR_Entry entries[4];
	uint16_t mbrSignature; // Valid bootsector signature (0x55, 0xAA)
} __attribute__((packed)) MBR_BootSector;

bool MBR_Parse(blockDevice* disk);

