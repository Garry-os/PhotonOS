// 
// VFS implementation for checking FS types
//
#include "vfs.h"
#include <qemu/print.h>

// This function only checks if it's FAT
// It doesn't check for FAT12/16/32
bool isFat(blockDevice* dev)
{
	uint8_t buffer[512];
	if (!dev->read(dev, 0, 1, buffer))
	{
		dbg_printf("[VFS] Failed to read lba 0 of %s\n", dev->name);
		return false;
	}

	bool ret = buffer[66] == 0x28 || buffer[66] == 0x29;
	return ret;
}

