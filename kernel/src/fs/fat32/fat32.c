//
// FAT32 read-only (RO) driver
//
#include "fat32.h"
#include <stdint.h>
#include <malloc.h>
#include <console.h>
#include <qemu/print.h>
#include <utils/memory.h>

fat32_data* g_data;

// Mount a partition block device
bool fat32_mount(blockDevice* dev)
{
	// Allocate memory
	g_data = (fat32_data*)malloc(sizeof(fat32_data));

	// Read the boot sector
	if (!dev->read(dev, 0, 1, g_data->bootSector.bytes))
	{
		goto error;
	}

	// Check for FAT signature (0x28 or 0x29)
	bool valid = g_data->bootSector.data.ebr.signature == 0x28 || g_data->bootSector.data.ebr.signature == 0x29;
	if (!valid)
	{
		dbg_printf("[FAT32] Invalid FAT32 signature on dev %s, signature: 0x%x\n", dev->name, g_data->bootSector.data.ebr.signature);
		goto error;
	}

	// Get FAT32 version
	g_data->minorVersion = g_data->bootSector.data.ebr.version & 0xFF;
	g_data->majorVersion = (g_data->bootSector.data.ebr.version >> 8) & 0xFF;

	// Calculate where FAT and cluster start
	g_data->fatStartLba = g_data->bootSector.data.reservedSectors;
	g_data->clusterStartLba = g_data->fatStartLba + (g_data->bootSector.data.ebr.sectorsPerFAT * g_data->bootSector.data.fatCount);

	g_data->dev = dev;

	printf("Mounted dev %s, FAT version %d.%d\n", dev->name, g_data->majorVersion, g_data->minorVersion);
	dbg_printf("[FAT32] fat start LBA: %d, cluster start LBA: %d\n", g_data->fatStartLba, g_data->clusterStartLba);
	return true;
error:
	printf("Failed to mount device %s\n", dev->name);
	return false;
}

// Return bytes read successfully
uint32_t fat32_read(const char* path, void* buffer)
{
	// Find the file first
	fat32DirEntry entry;
	if (!fat32_traverse(path, &entry))
	{
		return 0;
	}

	uint32_t remaining = entry.size;
	uint32_t cluster = (entry.firstClusterHigh << 16) | (entry.firstClusterLow & 0xFFFF);
	uint8_t* u8Buffer = (uint8_t*)buffer;
	uint8_t tmpBuffer[SECTOR_SIZE];

	while (cluster < FAT32_EOC && remaining > 0)
	{
		uint32_t lba = fat32_clusterToLba(cluster);
		
		for (int i = 0; i < g_data->bootSector.data.sectorsPerCluster && remaining > 0; i++)
		{
			if (!g_data->dev->read(g_data->dev, lba + i, 1, tmpBuffer))
			{
				dbg_printf("[FAT32] Failed to read LBA %d\n", lba);
				goto end;
			}

			uint8_t toCopy = remaining > SECTOR_SIZE ? SECTOR_SIZE : remaining;
			memcpy(u8Buffer, tmpBuffer, toCopy);
			u8Buffer += toCopy;
			remaining -= toCopy;
		}

		cluster = fat32_nextCluster(cluster);
	}

end:
	return u8Buffer - (uint8_t*)buffer;
}

