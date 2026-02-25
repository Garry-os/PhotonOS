//
// fat32.c: Traversing paths & finding entries
//
#include "fat32.h"
#include <qemu/print.h>
#include <utils/memory.h>

bool fat32_findEntry(uint32_t firstCluster, const char* name, fat32DirEntry* out)
{
	uint8_t tmpBuffer[SECTOR_SIZE];
	uint32_t cluster = firstCluster; // The current cluster
	
	// Get short name
	char shortName[12];
	fat32_NameToShort(name, shortName);

	while (cluster < FAT32_EOC && cluster != FAT32_BAD_CLUSTER && cluster != 0xFFFFFFFF)
	{
		// Loop through each sectors in a cluster
		for (int i = 0; i < g_data->bootSector.data.sectorsPerCluster; i++)
		{
			uint32_t lba = fat32_clusterToLba(cluster);
			if (!g_data->dev->read(g_data->dev, lba + i, 1, tmpBuffer))
			{
				dbg_printf("[FAT32] Failed to read LBA %x\n", lba);
				return false;
			}

			fat32DirEntry* entries = (fat32DirEntry*)tmpBuffer;
			// Loop through each entries in a sector
			for (int j = 0; j < SECTOR_SIZE / sizeof(fat32DirEntry); j++)
			{
				if (entries[j].name[0] == 0x00 ||
					entries[j].name[0] == 0xE5)
				{
					// No more entries
					// Deleted entry
					continue;
				}


				// Skip LFN & volume ID
				if (entries[j].attributes & FAT_ATTRIBUTE_VOLUME_ID ||
					entries[j].attributes & FAT_ATTRIBUTE_LFN)
				{
					continue;
				}

				fat32DirEntry entry = entries[j];

				// Compare the name
				if (memcmp(shortName, entry.name, 11) == 0)
				{
					// Found it
					*out = entry;
					return true;
				}
			}
		}
	}

	return false;
}


bool fat32_traverse(const char* path, fat32DirEntry* out)
{
	uint32_t cluster = g_data->bootSector.data.ebr.rootDirCluster;

	// Ignore leading slash
	if (path[0] == '/')
	{
		path++;
	}

	// TODO: Directory traversal
	
	fat32DirEntry entry;
	if (!fat32_findEntry(cluster, path, &entry))
	{
		dbg_printf("[FAT32] Directory entry not found: %s\n", path);
		return false;
	}

	*out = entry;
	return true;
}

