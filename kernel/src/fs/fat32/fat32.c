//
// FAT32 read-only (RO) driver
//
#include "fat32.h"
#include <stdint.h>
#include <malloc.h>
#include <console.h>
#include <qemu/print.h>
#include <utils/memory.h>
#include <utils/math.h>

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

fat32Handle* fat32_open(const char* path)
{
	// Traverse the file path
	fat32DirEntry entry;
	if (!fat32_traverse(path, &entry))
	{
		return NULL;
	}

	fat32Handle* handle = (fat32Handle*)malloc(sizeof(fat32Handle));
	handle->firstCluster = (entry.firstClusterHigh << 16) | (entry.firstClusterLow & 0xFFFF);
	handle->size = entry.size;
	handle->currentCluster = handle->firstCluster;
	handle->currentOffset = 0;

	return handle;
}

void fat32_close(fat32Handle* handle)
{
	free(handle);
	handle = NULL;
}

// Return bytes read successfully
uint32_t fat32_read(fat32Handle* handle, uint32_t limit, void* buffer)
{
	uint32_t remaining = min(limit, handle->size - handle->currentOffset);

	if (!remaining)
	{
		// EOF
		return 0;
	}

	uint32_t offset = handle->currentOffset;
	uint32_t cluster = handle->currentCluster;
	uint8_t* u8Buffer = (uint8_t*)buffer;
	uint8_t tmpBuffer[SECTOR_SIZE];

	uint32_t clusterSize = g_data->bootSector.data.sectorsPerCluster * SECTOR_SIZE;
	uint32_t skippedBytes = offset % clusterSize;

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

			uint8_t* tmpBufferPtr = tmpBuffer;

			// Calculate how many bytes to copy
			uint32_t toCopy = SECTOR_SIZE;
			if (skippedBytes >= SECTOR_SIZE)
			{
				skippedBytes -= SECTOR_SIZE;
				continue;
			}
			else if (skippedBytes > 0)
			{
				tmpBufferPtr += skippedBytes;
				toCopy -= skippedBytes;
				skippedBytes = 0;
			}

			toCopy = min(toCopy, remaining);
			memcpy(u8Buffer, tmpBufferPtr, toCopy);
			u8Buffer += toCopy;
			remaining -= toCopy;
			handle->currentOffset += toCopy;
		}

		cluster = fat32_nextCluster(cluster);
		handle->currentCluster = cluster;
	}

end:
	return u8Buffer - (uint8_t*)buffer;
}

