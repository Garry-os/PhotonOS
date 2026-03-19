// 
// FS syscalls like open(), close(), etc.
//
#include <user/syscalls.h>
#include <task.h>
#include <vfs/vfs.h>
#include <qemu/print.h>

#define SYSCALL_WRITE 1
size_t sys_write(int fd, const char* buffer, size_t count)
{
	if (count == 0)
	{
		return 0;
	}

	fileHandle* handle = currentTask->fds[fd];

	if (!handle)
	{
		// TODO: Implement UNIX error codes
		return 0;
	}

	return fsWrite(handle, count, buffer);
}

void SyscallRegisterFs()
{
	registerSyscall(SYSCALL_WRITE, sys_write);
}

