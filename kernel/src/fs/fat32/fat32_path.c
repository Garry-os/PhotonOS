//
// fat32.c: Traversing paths & finding entries
//
#include "fat32.h"
#include <qemu/print.h>
#include <utils/memory.h>
#include <utils/string.h>

bool fat32_findEntry(fat32Handle* handle, const char* name, fat32DirEntry* out)
{
	uint8_t nameBuffer[256];

	fat32DirEntry entry;
	while (fat32_readLFN(handle, nameBuffer, &entry))
	{
		if (strcmp((char*)nameBuffer, name) == 0)
		{
			*out = entry;
			return true;
		}
	}

	return false;
}


bool fat32_traverse(const char* path, fat32DirEntry* out)
{
	fat32DirEntry entry;

	// Check if it's root directory
	if (path[0] == '/' && path[1] == '\0')
	{
		return true;
	}

	// Ignore leading slash
	if (path[0] == '/')
	{
		path++;
	}

	// TODO: Directory traversal
	//
	
	fat32Handle* current = g_data->root; // Current directory
	
	if (!fat32_findEntry(current, path, &entry))
	{
		dbg_printf("[FAT32] Directory entry not found: %s\n", path);
		return false;
	}

	*out = entry;
	return true;
}

