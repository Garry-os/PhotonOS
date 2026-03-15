//
// /dev (devtmpfs) file system for managing devices
//
#include "dev.h"
#include <malloc.h>

size_t devOpen(fileHandle* handle, const char* path)
{
	(void)path;
	devfsFile* dev = (devfsFile*)handle->fileInfo;
	size_t result = dev->open(dev, handle);

	return result;
}

size_t devRead(fileHandle* handle, uint32_t limit, void* buffer)
{
	devfsFile* dev = (devfsFile*)handle->fileInfo;

	return dev->read(dev, limit, buffer);
}

void devRegister(devfsData* data, devfsFile* dev)
{
	data->dev[data->devCount++] = *dev;
}

bool devMount(mountpoint_t* mnt)
{
	// Setup mountpoint
	mnt->fsData = malloc(sizeof(devfsData));
	devfsData* devFs = (devfsData*)mnt->fsData;
	devFs->devCount = 0;

	devRegisterVfs(mnt);

	return true;
}

