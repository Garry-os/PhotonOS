//
// VFS main implementation file
//
#include "vfs.h"

mountpoint_t* firstMount = NULL;

fileHandle* fsOpen(const char* path)
{
	mountpoint_t* mnt = fsFindMnt(path);
	fileHandle* handle = NULL;

	if (mnt != NULL)
	{
		char* relPath = fsGetRelativePath(mnt, path);
		handle = mnt->ops->open(relPath);
		if (!handle)
		{
			return NULL;
		}

		// Setup file handle
		handle->ops = mnt->ops;
		handle->path = path;
		handle->mnt = mnt;
	}

	return handle;
}

size_t fsRead(fileHandle* handle, size_t limit, void* buffer)
{
	size_t bytesCount = handle->ops->read(handle, limit, buffer);
	return bytesCount;
}

void fsClose(fileHandle* handle)
{
	handle->ops->close(handle);
}

