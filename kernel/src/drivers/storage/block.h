#pragma once
#include <stdint.h>
#include <stddef.h>

// Block device type (e.g NVME)
typedef enum
{
	BLOCK_TYPE_SATA,
	BLOCK_TYPE_ATA,
	BLOCK_TYPE_NVME,
	BLOCK_TYPE_NONE
} BLOCK_TYPE;

typedef struct blockDevice
{
	struct blockDevice* next;

	bool (*read)(struct blockDevice* block, uint64_t lba, uint32_t count, void* buffer);

	char modelName[39];
	size_t sectorCount; // Total sectors of the drive
	uint8_t type;

	void* driverPtr; // e.g ahciDevice struct
} blockDevice;

extern blockDevice* firstBlock;


