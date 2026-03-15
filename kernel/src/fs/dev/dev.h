#pragma once
#include <stdbool.h>
#include <vfs/vfs.h>

#define DEV_MAX_DEVICES 32

typedef struct
{
	size_t (*open)(void* dev, fileHandle* handle);
	size_t (*read)(void* dev, uint32_t limit, void* buffer);
	const char* name;
} devfsFile;

typedef struct
{
	devfsFile dev[DEV_MAX_DEVICES];
	int devCount;
} devfsData;

void devRegisterVfs(mountpoint_t* mnt);

size_t devOpen(fileHandle* handle, const char* path);
size_t devRead(fileHandle* handle, uint32_t limit, void* buffer);

void devRegister(devfsData* data, devfsFile* dev);
bool devMount(mountpoint_t* mnt);

