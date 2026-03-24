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
#include <user/uapi.h>

// Mount a partition block device
bool fat32_mount(mountpoint_t* mnt)
{
	blockDevice* dev = mnt->dev;

	// Setup mount point
	mnt->fsData = malloc(sizeof(fat32_data));
	fat32_registerVFS(mnt);

	fat32_data* data = (fat32_data*)mnt->fsData;

	// Read the boot sector
	if (!dev->read(dev, 0, 1, data->bootSector.bytes))
	{
		goto error;
	}

	// Check for FAT signature (0x28 or 0x29)
	bool valid = data->bootSector.data.ebr.signature == 0x28 || data->bootSector.data.ebr.signature == 0x29;
	if (!valid)
	{
		dbg_printf("[FAT32] Invalid FAT32 signature on dev %s, signature: 0x%x\n", dev->name, data->bootSector.data.ebr.signature);
		goto error;
	}

	// Get FAT32 version
	data->minorVersion = data->bootSector.data.ebr.version & 0xFF;
	data->majorVersion = (data->bootSector.data.ebr.version >> 8) & 0xFF;

	// Calculate where FAT and cluster start
	data->fatStartLba = data->bootSector.data.reservedSectors;
	data->clusterStartLba = data->fatStartLba + (data->bootSector.data.ebr.sectorsPerFAT * data->bootSector.data.fatCount);

	data->dev = dev;

	// Setup the root directory file
	data->root = (fat32Handle*)malloc(sizeof(fat32Handle));
	data->root->firstCluster = data->bootSector.data.ebr.rootDirCluster;
	data->root->currentSector = data->clusterStartLba;
	data->root->size = sizeof(fat32DirEntry) * data->bootSector.data.rootDirEntries;
	data->root->currentCluster = data->root->firstCluster;
	data->root->currentOffset = 0;
	data->root->attributes = FAT_HANDLE_ROOT | FAT_HANDLE_DIR;

	// Read the first cluster into the buffer
	if (!data->dev->read(data->dev, fat32_clusterToLba(data, data->bootSector.data.ebr.rootDirCluster), 1, data->root->buffer))
	{
		dbg_printf("[FAT32] Failed to read LBA %\n", data->clusterStartLba);
		return false;
	}


	printf("Mounted dev %s, FAT version %d.%d\n", dev->name, data->majorVersion, data->minorVersion);
	dbg_printf("[FAT32] fat start LBA: %d, cluster start LBA: %d\n", data->fatStartLba, data->clusterStartLba);
	return true;
error:
	printf("Failed to mount device %s\n", dev->name);
	return false;
}

fat32Handle* fat32_open(fat32_data* data, const char* path)
{
	// Traverse the file path
	fat32DirEntry entry;
	if (!fat32_traverse(data, path, &entry))
	{
		return NULL;
	}

	// Check if it's root directory
	if (path[0] == '/' && path[1] == '\0')
	{
		fat32_close(data, data->root);
		return data->root;
	}

	return fat32_openEntry(data, entry);
}

fat32Handle* fat32_openEntry(fat32_data* data, fat32DirEntry entry)
{
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
	if (!data->dev->read(data->dev, fat32_clusterToLba(data, handle->currentCluster), 1, handle->buffer))
	{
		dbg_printf("[FAT32] Failed to read LBA %d\n", fat32_clusterToLba(data, handle->currentCluster));
		fat32_close(data, handle);
		return NULL;
	}

	return handle;
}

void fat32_close(fat32_data* data, fat32Handle* handle)
{
	if ((handle->attributes & FAT_HANDLE_ROOT) > 0)
	{
		// Root directory can't be close
		// Only reset it here
		handle->currentCluster = handle->firstCluster;
		handle->currentOffset = 0;
		handle->currentSector = data->clusterStartLba;
		return;
	}

	free(handle);
	handle = NULL;
}

// Return bytes read successfully
uint32_t fat32_read(fat32_data* data, fat32Handle* handle, uint32_t limit, void* buffer)
{
	uint8_t* u8Buffer = (uint8_t*)buffer;

	uint32_t remaining = limit;
	uint32_t clusterOffset = handle->clusterOffset;

	// Directory size is 0 (in some cases)
	if (!(handle->attributes & FAT_HANDLE_DIR) || ((handle->attributes & FAT_HANDLE_DIR) > 0 && handle->size != 0))
	{
		remaining = min(remaining, handle->size - handle->currentOffset);
	}

	uint32_t sectorOffset = handle->clusterOffset / SECTOR_SIZE;
	handle->currentSector = sectorOffset % data->bootSector.data.sectorsPerCluster;
	
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

				if (!data->dev->read(data->dev, handle->currentSector, 1, handle->buffer))
				{
					dbg_printf("[FAT32] Failed to read LBA %d\n", handle->currentSector);
					goto end;
				}
			}
			else
			{
				// Calculate & read next sector
				if (++handle->currentSector >= data->bootSector.data.sectorsPerCluster)
				{
					// Ran out of sectors in a cluster
					// Need to calculate for the next cluster in order to read
					handle->currentSector = 0;
					handle->currentCluster = fat32_nextCluster(data, handle->currentCluster);
				}

				// Check for no more clusters & bad cluster
				if (handle->currentCluster >= FAT32_EOC || handle->currentCluster == FAT32_BAD_CLUSTER)
				{
					// EOF
					handle->size = handle->currentOffset;
					goto end;
				}

				// Read next sector
				uint32_t lba = handle->currentSector + fat32_clusterToLba(data, handle->currentCluster);
				if (!data->dev->read(data->dev, lba, 1, handle->buffer))
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
bool fat32_readEntry(fat32_data* data, fat32Handle* handle, fat32DirEntry* entry)
{
	bool success = fat32_read(data, handle, sizeof(fat32DirEntry), entry) == sizeof(fat32DirEntry);
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
ssize_t fat32_readLFN(fat32_data* data, fat32Handle* handle, uint8_t* buffer, fat32DirEntry* out)
{
	if (!(handle->attributes & FAT_HANDLE_DIR))
	{
		return -ENOTDIR;
	}

	int lfnLast = -1;
	fat32DirEntry entry;
	while (fat32_readEntry(data, handle, &entry))
	{
		fat32LFNEntry* lfn = (fat32LFNEntry*)&entry;
		if (entry.attributes == FAT_ATTRIBUTE_LFN && !lfn->type)
		{
			int index = (lfn->order & ~FAT32_ORDER_LAST) - 1;

			// If LFN index is valid
			if (index > FAT32_LFN_MAX_INDEX)
			{
				dbg_printf("[FAT32] Invalid LFN index: %d\n", index);
				return -EIO;
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
				return -EIO;
			}

			if (out)
				*out = entry;

			return 0;
		}
	}

	return 1; // End of directories
}

ssize_t fat32_seek(fat32_data* data, fat32Handle* handle, uint32_t offset)
{
	if (offset > handle->size)
	{
		return -EINVAL;
	}

	if (handle->attributes & FAT_HANDLE_DIR)
	{
		return -EINVAL;
	}

	if (offset == handle->currentOffset)
	{
		return offset;
	}

	uint32_t clusterSize = data->bootSector.data.sectorsPerCluster * SECTOR_SIZE;
	uint32_t clusterIndex = offset / clusterSize;
	uint32_t clusterOffset = offset % clusterSize;
	uint32_t currentCluster = handle->firstCluster;

	uint32_t sectorOffset = clusterOffset / SECTOR_SIZE;

	// Calculate the cluster using the cluster index
	for (uint32_t i = 0; i < clusterIndex; i++)
	{
		currentCluster = fat32_nextCluster(data, currentCluster);
		if (currentCluster >= FAT32_EOC || currentCluster == FAT32_BAD_CLUSTER)
		{
			return 0;
		}
	}

	// Update values
	handle->currentCluster = currentCluster;
	handle->currentOffset = offset;
	handle->currentSector = sectorOffset;
	handle->clusterOffset = clusterOffset;

	// Update the sector buffer
	uint32_t lba = handle->currentSector + fat32_clusterToLba(data, handle->currentCluster);
	if (!data->dev->read(data->dev, lba, 1, handle->buffer))
	{
		dbg_printf("[FAT32] Failed to read LBA %d\n", lba);
		return 0;
	}

	return (ssize_t)offset;
}


