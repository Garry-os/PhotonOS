#pragma once
#include <stdint.h>
#include <stddef.h>

typedef enum
{
	DEV_TYPE_DISK,
	DEV_TYPE_PARTITION,
	DEV_TYPE_NONE
} DEV_TYPE;

typedef struct blockDevice
{
	struct blockDevice* next;

	struct blockDevice* parent;

	bool (*read)(struct blockDevice* block, uint64_t lba, uint32_t count, void* buffer);

	uint8_t type;
	char modelName[39];
	char* name; // e.g sda
	size_t sectorCount; // Total sectors of the drive
	uint64_t lbaStart; // For partitions

	void* driverPtr; // e.g ahciDevice struct
} blockDevice;

extern blockDevice* firstBlock;

void blockRegister(blockDevice* block);


