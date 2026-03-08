//
// FAT32 VFS interface adapter
// Allows VFS to "communicate" with FAT32 without FAT32 changing the core driver
//
#include "fat32.h"
#include <qemu/print.h>

size_t fat32_vfs_open(fileHandle* handle, const char* path);
void fat32_vfs_close(fileHandle* handle);

size_t fat32_vfs_read(fileHandle* handle, uint32_t limit, void* buffer);
size_t fat32_vfs_getFileSize(fileHandle* handle);

fs_ops_t fat32_ops = {
	.open = fat32_vfs_open,
	.close = fat32_vfs_close,

	.read = fat32_vfs_read,
	.getFileSize = fat32_vfs_getFileSize
};

void fat32_registerVFS(mountpoint_t* mnt)
{
	mnt->ops = &fat32_ops;
}

size_t fat32_vfs_open(fileHandle* handle, const char* path)
{
	fat32Handle* fatHandle = NULL;
	fat32_data* data = (fat32_data*)handle->mnt->fsData;

	fatHandle = fat32_open(data, path);
	
	if (!fatHandle)
	{
		return 1;
	}

	handle->fileInfo = (void*)fatHandle;

	return 0;
}

void fat32_vfs_close(fileHandle* handle)
{
	fat32Handle* fatHandle = (fat32Handle*)handle->fileInfo;
	fat32_data* data = (fat32_data*)handle->mnt->fsData;

	fat32_close(data, fatHandle);
}

size_t fat32_vfs_read(fileHandle* handle, uint32_t limit, void* buffer)
{
	fat32Handle* fatHandle = (fat32Handle*)handle->fileInfo;
	fat32_data* data = (fat32_data*)handle->mnt->fsData;

	size_t bytesRead = fat32_read(data, fatHandle, limit, buffer);
	return bytesRead;
}

size_t fat32_vfs_getFileSize(fileHandle* handle)
{
	fat32Handle* fatHandle = (fat32Handle*)handle->fileInfo;
	return (size_t)fatHandle->size;
}

