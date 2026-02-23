#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct blockDevice
{
	struct blockDevice* next;

	bool (*read)(struct blockDevice* block, uint64_t lba, uint32_t count, void* buffer);

	char modelName[39];
	char* name; // e.g sda
	size_t sectorCount; // Total sectors of the drive

	void* driverPtr; // e.g ahciDevice struct
} blockDevice;

extern blockDevice* firstBlock;

void blockRegister(blockDevice* block);


