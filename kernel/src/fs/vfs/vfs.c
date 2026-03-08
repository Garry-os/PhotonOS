//
// VFS main implementation file
//
#include "vfs.h"
#include <malloc.h>
#include <utils/string.h>
#include <qemu/print.h>

mountpoint_t* firstMount = NULL;

fileHandle* fsOpen(const char* path)
{
	mountpoint_t* mnt = fsFindMnt(path);
	fileHandle* handle = (fileHandle*)malloc(sizeof(handle));

	handle->path = (char*)malloc((size_t)strlen(path));

	if (mnt != NULL)
	{
		// Setup file handle
		handle->ops = mnt->ops;
		strcpy(handle->path, path);
		handle->mnt = mnt;

		char* relPath = fsGetRelativePath(mnt, path);
		size_t result = mnt->ops->open(handle, relPath);
		if (result != 0)
		{
			dbg_printf("[VFS] Failed to open file: %s\n", path);
			free(handle->path);
			free(handle);
			return NULL;
		}
	}
	else
	{
		return NULL;
	}

	return handle;
}

size_t fsRead(fileHandle* handle, size_t limit, void* buffer)
{
	size_t bytesCount = handle->ops->read(handle, limit, buffer);
	return bytesCount;
}

size_t fsGetFileSize(fileHandle* handle)
{
	return handle->ops->getFileSize(handle);
}

// Return bytes sought
size_t fsSeek(fileHandle* handle, int offset, int whence)
{
	int target = offset;
	if (whence == SEEK_SET)
	{
		target += 0; // Current
	}
	else if (whence == SEEK_CUR)
	{
		target += handle->current; // Current + offset
	}
	else if (whence == SEEK_END)
	{
		target += fsGetFileSize(handle);
	}

	size_t result = handle->ops->seek(handle, target);
	return result;
}

dirent64* fsReaddir(fileHandle* handle, dirent64* dir)
{
	bool success = handle->ops->readdir(handle, (uint8_t*)dir->name);
	if (!success)
	{
		return NULL;
	}

	return dir;
}

void fsClose(fileHandle* handle)
{
	handle->ops->close(handle);
	free(handle->path);
	free(handle);
}

