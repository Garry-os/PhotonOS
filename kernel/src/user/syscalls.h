#pragma once

void registerSyscall(int num, void* entry);

void InitSyscall();

// syscalls/fs.c
void SyscallRegisterFs();

