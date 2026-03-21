// 
// FS syscalls like open(), close(), etc.
//
#include <user/syscalls.h>
#include <task.h>
#include <vfs/vfs.h>
#include <qemu/print.h>

#define SYSCALL_WRITE 1
ssize_t sys_write(int fd, const char* buffer, size_t count)
{
	if (count == 0)
	{
		return 0;
	}

	fileHandle* handle = currentTask->fds[fd];

	if (!handle)
	{
		return -EBADF;
	}

	return fsWrite(handle, count, buffer);
}

void SyscallRegisterFs()
{
	registerSyscall(SYSCALL_WRITE, sys_write);
}

