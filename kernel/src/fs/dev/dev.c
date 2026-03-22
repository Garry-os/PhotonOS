//
// /dev (devtmpfs) file system for managing devices
//
#include "dev.h"
#include <malloc.h>
#include <utils/string.h>
#include <qemu/print.h>
#include <utils/memory.h>

extern fs_ops_t stdioOps;

ssize_t devOpen(fileHandle* handle, const char* path)
{
	// Skip leading slash
	if (path[0] == '/')
	{
		path++;
	}

	devfsFile* dev = NULL;
	devfsData* data = (devfsData*)handle->mnt->fsData;

	// Loop through and find the device
	for (int i = 0; i < DEV_MAX_DEVICES; i++)
	{
		if (strcmp(path, data->dev[i].name) == 0)
		{
			dev = &data->dev[i];
			break;
		}
	}

	if (!dev)
	{
		dbg_printf("[Dev] Device not found: %s\n", path);
		return -ENOENT;
	}

	handle->fileInfo = (void*)dev;

	size_t result = 0;
	if (dev->ops->open)
	{
		result = dev->ops->open(handle, path);
	}

	return result;
}

ssize_t devRead(fileHandle* handle, uint32_t limit, void* buffer)
{
	devfsFile* dev = (devfsFile*)handle->fileInfo;

	if (!dev->ops->read)
	{
		return -EBADF;
	}

	return dev->ops->read(handle, limit, buffer);
}

ssize_t devWrite(fileHandle* handle, uint32_t limit, void* buffer)
{
	devfsFile* dev = (devfsFile*)handle->fileInfo;

	if (!dev->ops->write)
	{
		return -EBADF;
	}

	return dev->ops->write(handle, limit, buffer);
}

void devClose(fileHandle* handle)
{
	devfsFile* dev = (devfsFile*)handle->fileInfo;
	free(dev->name);
	// TODO: Remove dev from data->dev!
	if (dev->ops->close)
	{
		dev->ops->close(handle);
	}
}

void devRegister(devfsData* data, fs_ops_t* ops, const char* name)
{
	// Setup
	devfsFile dev;
	dev.ops = ops;
	dev.name = (char*)malloc(strlen(name) + 1);
	strcpy(dev.name, name);

	data->dev[data->devCount++] = dev;
}

bool devMount(mountpoint_t* mnt)
{
	// Setup mountpoint
	mnt->fsData = malloc(sizeof(devfsData));
	devfsData* devFs = (devfsData*)mnt->fsData;
	devFs->devCount = 0;

	devRegisterVfs(mnt);

	// Register stdout, stdin, stderr
	devRegister(devFs, &stdioOps, "stdout");
	devRegister(devFs, &stdioOps, "stdin");
	devRegister(devFs, &stdioOps, "stderr");

	return true;
}

