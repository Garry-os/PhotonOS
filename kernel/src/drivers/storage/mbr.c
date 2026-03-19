//
// MBR partition block device parsing
//
#include "mbr.h"
#include <qemu/print.h>
#include <x86_64/cpu.h>
#include <malloc.h>
#include <utils/memory.h>

static int namingOffset = 1; // Start with sda1

bool MBR_ReadBlock(blockDevice* block, uint64_t lba, uint32_t count, void* buffer)
{
	if (!block->parent)
	{
		dbg_printf("[MBR] No block device parent (%s)!\n", block->name);
		return false;
	}

	return block->parent->read(block->parent, lba + block->lbaStart, count, buffer);
}

static char* generateBlockName(char* parentName)
{
	char* name = malloc(11 * sizeof(char));
	memset(name, 0, 11 * sizeof(char));

	// Copy parent's name
	int i = 0;
	while (parentName[i] != '\0')
	{
		name[i] = parentName[i];
		i++;
	}

	name[i++] = (char)namingOffset + '0';
	name[i++] = 0;

	return name;
}

// Return if disk is MBR
bool MBR_Parse(blockDevice* disk)
{
	// Read the first sector
	uint8_t buffer[512];
	if (!disk->read(disk, 0, 1, buffer))
	{
		dbg_printf("[MBR] Failed to read MBR bootsector (LBA 0x0)\n");
		panic();
	}

	MBR_BootSector* mbr = (MBR_BootSector*)buffer;

	// Validate MBR
	if (buffer[510] != 0x55 || buffer[511] != 0xAA)
	{
		return false;
	}

	// Scan & validate each MBR partitions' entries
	for (int i = 0; i < 4; i++)
	{
		MBR_Entry entry = mbr->entries[i];

		if (entry.type != 0 && entry.sectorsNum != 0)
		{
			// Valid entry
			// Create new block device
			blockDevice* newBlock = (blockDevice*)malloc(sizeof(blockDevice));
			memset(newBlock, 0, sizeof(blockDevice));
			newBlock->parent = disk;
			newBlock->lbaStart = entry.lbaStart;
			newBlock->read = MBR_ReadBlock;
			newBlock->name = generateBlockName(disk->name);
			newBlock->type = DEV_TYPE_PARTITION;
			newBlock->sectorCount = entry.sectorsNum;
			memcpy(newBlock->modelName, disk->modelName, 46 * 2 - 27 * 2); // Copy the model name

			blockRegister(newBlock);
		}
	}

	return true;
}

