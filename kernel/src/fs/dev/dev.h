#pragma once
#include <stdbool.h>
#include <vfs/vfs.h>

#define DEV_MAX_DEVICES 32

typedef struct
{
	fs_ops_t* ops;
	int dirOffset; // For readdir
	char* name;
} devfsFile;

typedef struct
{
	devfsFile dev[DEV_MAX_DEVICES];
	int devCount;
} devfsData;

void devRegisterVfs(mountpoint_t* mnt);

ssize_t devOpen(fileHandle* handle, const char* path);

size_t devRead(fileHandle* handle, uint32_t limit, void* buffer);
size_t devWrite(fileHandle* handle, uint32_t limit, void* buffer);
bool devReaddir(fileHandle* handle, uint8_t* buffer);

void devClose(fileHandle* handle);

void devRegister(devfsData* data, fs_ops_t* ops, const char* name);
bool devMount(mountpoint_t* mnt);

