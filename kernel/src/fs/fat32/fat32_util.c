#include "fat32.h"
#include <qemu/print.h>
#include <utils/memory.h>
#include <utils/ctype.h>
#include <utils/string.h>

uint32_t fat32_clusterToLba(fat32_data* data, uint32_t cluster)
{
	return data->clusterStartLba + (cluster - 2) * data->bootSector.data.sectorsPerCluster;
}

uint32_t fat32_nextCluster(fat32_data* data, uint32_t cluster)
{
	uint32_t fatIndex = cluster * 4;
	uint8_t tmpBuffer[SECTOR_SIZE];
	uint32_t lba = data->fatStartLba + (fatIndex / SECTOR_SIZE);

	// Read in the FAT table
	if (!data->dev->read(data->dev, lba, 1, tmpBuffer))
	{
		dbg_printf("Failed to read!\n");
		return 0xFFFFFFFF;
	}

	uint32_t offset = fatIndex % SECTOR_SIZE;
	uint32_t next = *(uint32_t*)&tmpBuffer[offset];
	next &= 0x0FFFFFFF; // Upper 4 bits are reserved, it is FAT28 lol
	
	return next;
}

// Convert from normal name to FAT short name
void fat32_NameToShort(const char* name, char shortName[12])
{
	memset(shortName, ' ', 12);
	shortName[11] = '\0';

	const char* ext = strchr(name, '.'); // Look for the '.' extension
	if (!ext)
	{
		ext = name + 11;
	}

	// Copy the 8 chars in 8.3
	for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
	{
		shortName[i] = toupper(name[i]);
	}

	// Copy the 3 chars in 8.3
	if (ext != name + 11)
	{
		for (int i = 0; i < 3 && ext[i + 1]; i++)
		{
			shortName[i + 8] = toupper(ext[i + 1]);
		}
	}
}

// Copy the LFN's chars into a buffer
void fat32_copyLFN(uint8_t* buffer, fat32LFNEntry* lfn, int index)
{
	uint8_t* target = &buffer[index * 13]; // 5 + 6 + 2 = 13 (total characters)
	int current = 0;

	// Copy the characters
	for (int i = 0; i < 5; i++)
	{
		target[current++] = lfn->chars1[i];
	}

	for (int i = 0; i < 6; i++)
	{
		target[current++] = lfn->chars2[i];
	}

	for (int i = 0; i < 2; i++)
	{
		target[current++] = lfn->chars3[i];
	}
}

