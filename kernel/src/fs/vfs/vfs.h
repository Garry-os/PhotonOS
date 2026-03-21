#pragma once
#include <stddef.h>
#include <storage/block.h>
#include <stdbool.h>
#include <uapi.h>
#include <lock.h>

#define VFS_MAX_PATH_LEN 128

// FS types
typedef enum
{
	FS_TYPE_FAT,
	FS_TYPE_DEV,
	FS_TYPE_NONE
} FS_TYPE;

typedef struct fileHandle fileHandle;

// FS-specific operations like read, write, open, etc.
// FS driver should provide those functions.
typedef struct
{
	ssize_t (*open)(fileHandle* handle, const char* path);
	void (*close)(fileHandle* handle);

	size_t (*read)(fileHandle* handle, uint32_t limit, void* buffer);
	size_t (*write)(fileHandle* handle, uint32_t limit, void* buffer);

	size_t (*getFileSize)(fileHandle* handle);
	size_t (*seek)(fileHandle* handle, int offset);
	bool   (*readdir)(fileHandle* handle, uint8_t* buffer);
} fs_ops_t;

typedef struct mountpoint
{
	struct mountpoint* next;

	uint8_t fsType; // FS type
	char prefix[VFS_MAX_PATH_LEN];
	blockDevice* dev;

	fs_ops_t* ops;
	void* fsData;
} mountpoint_t;

extern mountpoint_t* firstMount;

struct fileHandle
{
	char* path;

	fs_ops_t* ops;

	mountpoint_t* mnt;
	size_t current; // Current reading offset
	void* fileInfo;
};

ssize_t fsOpen(const char* path, fileHandle** out);

size_t fsRead(fileHandle* handle, size_t limit, void* buffer);
size_t fsWrite(fileHandle* handle, size_t limit, void* buffer);

size_t fsGetFileSize(fileHandle* handle);
size_t fsSeek(fileHandle* handle, int offset, int whence);
dirent64* fsReaddir(fileHandle* handle, dirent64* dir);

void fsClose(fileHandle* handle);

// vfs_mnt.c
mountpoint_t* fsMount(blockDevice* src, uint8_t fsType, const char* prefix);
void fsUnmount(mountpoint_t* mnt);

mountpoint_t* fsFindMnt(const char* prefix);
char* fsGetRelativePath(mountpoint_t* mnt, const char* path);

// vfs_fs.c
bool isFat(blockDevice* dev);

