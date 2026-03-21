#pragma once
#include <user/uapi.h>

void registerSyscall(int num, void* entry);

void InitSyscall();

// syscalls/fs.c
void SyscallRegisterFs();

