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

	fat32Handle* current = g_data->root; // Current directory
	bool isLast = false;

	char name[256];

	while (*path)
	{
		const char* delim = strchr(path, '/');
		if (delim != NULL)
		{
			// Still more subdirectories...
			memcpy(name, path, delim - path);
			name[delim - path] = '\0';
			path = delim + 1;
		}
		else
		{
			// Reached the file, or directory
			unsigned len = strlen(path);
			memcpy(name, path, len);
			name[len] = '\0';
			path += len;
			isLast = true;
		}

		if (!fat32_findEntry(current, name, &entry))
		{
			dbg_printf("[FAT32] Directory entry not found: %s\n", name);
			fat32_close(current);
			return false;
		}

		fat32_close(current);

		// Check if it's a directory
		if (!isLast && !(current->attributes & FAT_HANDLE_DIR))
		{
			fat32_close(current);
			dbg_printf("[FAT32] %s isn't a directory\n", name);
			return false;
		}

		// Open a new directory
		current = fat32_openEntry(entry);
	}

	*out = entry;
	return true;
}

