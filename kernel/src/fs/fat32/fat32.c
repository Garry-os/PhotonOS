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

	// Setup the root directory file
	g_data->root = (fat32Handle*)malloc(sizeof(fat32Handle));
	g_data->root->firstCluster = g_data->bootSector.data.ebr.rootDirCluster;
	g_data->root->currentSector = g_data->clusterStartLba;
	g_data->root->size = sizeof(fat32DirEntry) * g_data->bootSector.data.rootDirEntries;
	g_data->root->currentCluster = g_data->root->firstCluster;
	g_data->root->currentOffset = 0;
	g_data->root->attributes = FAT_HANDLE_ROOT | FAT_HANDLE_DIR;

	// Read the first cluster into the buffer
	if (!g_data->dev->read(g_data->dev, fat32_clusterToLba(g_data->bootSector.data.ebr.rootDirCluster), 1, g_data->root->buffer))
	{
		dbg_printf("[FAT32] Failed to read LBA %\n", g_data->clusterStartLba);
		return false;
	}

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

	// Check if it's root directory
	if (path[0] == '/' && path[1] == '\0')
	{
		fat32_close(g_data->root);
		return g_data->root;
	}

	fat32Handle* handle = (fat32Handle*)malloc(sizeof(fat32Handle));
	handle->firstCluster = (entry.firstClusterHigh << 16) | (entry.firstClusterLow & 0xFFFF);
	handle->size = entry.size;
	handle->currentCluster = handle->firstCluster;
	handle->currentOffset = 0;
	handle->currentSector = 0;
	handle->attributes = 0;

	if ((entry.attributes & FAT_ATTRIBUTE_DIRECTORY) > 0)
	{
		handle->attributes |= FAT_HANDLE_DIR;
	}

	// Read the first cluster into the buffer
	if (!g_data->dev->read(g_data->dev, fat32_clusterToLba(handle->currentCluster), 1, handle->buffer))
	{
		dbg_printf("[FAT32] Failed to read LBA %d\n", fat32_clusterToLba(handle->currentCluster));
		fat32_close(handle);
		return NULL;
	}

	return handle;
}

void fat32_close(fat32Handle* handle)
{
	if ((handle->attributes & FAT_HANDLE_ROOT) > 0)
	{
		// Root directory can't be close
		// Only reset it here
		handle->currentCluster = handle->firstCluster;
		handle->currentOffset = 0;
		handle->currentSector = g_data->clusterStartLba;
		return;
	}

	free(handle);
	handle = NULL;
}

// Return bytes read successfully
uint32_t fat32_read(fat32Handle* handle, uint32_t limit, void* buffer)
{
	uint8_t* u8Buffer = (uint8_t*)buffer;

	uint32_t remaining = limit;

	// Directory size is 0 (in some cases)
	if (!(handle->attributes & FAT_HANDLE_DIR) || ((handle->attributes & FAT_HANDLE_DIR) > 0 && handle->size != 0))
	{
		remaining = min(remaining, handle->size - handle->currentOffset);
	}
	
	while (remaining > 0)
	{
		uint32_t leftOffset = handle->currentOffset % SECTOR_SIZE;
		uint32_t left = SECTOR_SIZE - leftOffset; // How many bytes left in a sector?
		uint32_t take = min(remaining, left);

		// Copy the data into the buffer
		memcpy(u8Buffer, handle->buffer + leftOffset, take);
		u8Buffer += take;
		handle->currentOffset += take;
		remaining -= take;

		// If we need to read more sectors?
		if (left == take)
		{
			if ((handle->attributes & FAT_HANDLE_ROOT) > 0)
			{
				handle->currentSector++;

				if (!g_data->dev->read(g_data->dev, handle->currentSector, 1, handle->buffer))
				{
					dbg_printf("[FAT32] Failed to read LBA %d\n", handle->currentSector);
					goto end;
				}
			}
			else
			{
				// Calculate & read next sector
				if (++handle->currentSector >= g_data->bootSector.data.sectorsPerCluster)
				{
					// Ran out of sectors in a cluster
					// Need to calculate for the next cluster in order to read
					handle->currentSector = 0;
					handle->currentCluster = fat32_nextCluster(handle->currentCluster);
				}

				// Check for no more clusters & bad cluster
				if (handle->currentCluster >= FAT32_EOC || handle->currentCluster == FAT32_BAD_CLUSTER)
				{
					// EOF
					handle->size = handle->currentOffset;
					goto end;
				}

				// Read next sector
				uint32_t lba = handle->currentSector + fat32_clusterToLba(handle->currentCluster);
				if (!g_data->dev->read(g_data->dev, lba, 1, handle->buffer))
				{
					dbg_printf("[FAT32] Failed to read lba %d\n", lba);
					goto end;
				}
			}
		}
	}
	

end:
	return u8Buffer - (uint8_t*)buffer;
}

// Read a FAT32 entry
// the LFN entry argument can be NULL
bool fat32_readEntry(fat32Handle* handle, fat32DirEntry* entry)
{
	bool success = fat32_read(handle, sizeof(fat32DirEntry), entry) == sizeof(fat32DirEntry);
	if (!success)
	{
		return false;
	}

	if (entry->name[0] == 0x00 || entry->name[0] == 0xE5)
	{
		// No more entries
		return false;
	}

	return true;
}

// Loops over the LFN entries until the last entry
// And copy the names into a buffer
// buffer must be >= 256
// entry out is optional, it can be NULL
bool fat32_readLFN(fat32Handle* handle, uint8_t* buffer, fat32DirEntry* out)
{
	int lfnLast = -1;
	fat32DirEntry entry;
	while (fat32_readEntry(handle, &entry))
	{
		fat32LFNEntry* lfn = (fat32LFNEntry*)&entry;
		if (entry.attributes == FAT_ATTRIBUTE_LFN && !lfn->type)
		{
			int index = (lfn->order & ~FAT32_ORDER_LAST) - 1;

			// If LFN index is valid
			if (index > FAT32_LFN_MAX_INDEX)
			{
				dbg_printf("[FAT32] Invalid LFN index: %d\n", index);
				return false;
			}

			// Check if it's the last index
			if ((lfn->order & FAT32_ORDER_LAST) > 0)
			{
				lfnLast = index;
			}

			// Copy over the LFN entry's name
			fat32_copyLFN(buffer, lfn, index);

			continue;
		}

		if (lfnLast >= 0)
		{
			// LFN entries reading is completed
			// Calculate the LFN's length
			int lfnLen = 0;
			while (buffer[lfnLen++]);

			lfnLen--; // NULL termination
			if (lfnLen < 0)
			{
				dbg_printf("[FAT32] Invalid LFN length: %d\n", lfnLen);
				return false;
			}

			if (out)
				*out = entry;

			return true;
		}
	}

	return false;
}


