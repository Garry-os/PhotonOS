//
// VFS mounting, unmounting
// and linked list mountpoints managements
//
#include "vfs.h"
#include <utils/llist.h>
#include <utils/string.h>
#include <utils/memory.h>
#include <fat32/fat32.h>
#include <qemu/print.h>

mountpoint_t* fsMount(char* prefix, blockDevice* dev)
{
	lockAcquire();
	mountpoint_t* newMnt = LL_Allocate((void**)&firstMount, sizeof(mountpoint_t));
	lockRelease();

	// Copy over the mount info
	strcpy(newMnt->prefix, prefix); // Copy over the mount prefix
	newMnt->dev = dev;

	// Determine file system
	bool success = false;
	if (isFat(dev))
	{
		newMnt->fsType = FS_TYPE_FAT;
		success = fat32_mount(newMnt);
	}
	else
	{
		dbg_printf("[VFS] No filesystem found on %s\n", dev->name);
	}
	
	if (!success)
	{
		fsUnmount(newMnt);
		return NULL;
	}

	return newMnt;
}

void fsUnmount(mountpoint_t* mnt)
{
	(void)mnt;
	dbg_printf("[VFS] TODO: fsUnmount()\n");
}

// Returns a mountpoint based on the prefix
mountpoint_t* fsFindMnt(const char* prefix)
{
	mountpoint_t* current = firstMount;
	unsigned largestLen = 0;
	mountpoint_t* target = NULL;

	// Loop over all mountpoints
	// Try to find the prefix with the most matchup
	while (current)
	{
		unsigned len = strlen(current->prefix) - 1; // Without the leading slash
		if (len >= largestLen && memcmp(current->prefix, prefix, len) == 0 &&
			(prefix[len] == '/' || prefix[len] == '\0'))
		{
			largestLen = len;
			target = current;
		}

		current = current->next;
	}

	if (!target)
	{
		dbg_printf("[VFS] No mountpoint found: %s\n", prefix);
		return NULL;
	}

	return target;
}

// /mnt/home/hello.txt -> /home/hello.txt
char* fsGetRelativePath(mountpoint_t* mnt, const char* path)
{
	char* newPath = (char*)((uint64_t)path + strlen(mnt->prefix));

	// E.g path = /mnt
	// Then the return value is NULL
	// Checking that helps fix it
	if (newPath[0] == '\0')
	{
		return "/";
	}

	return newPath;
}

