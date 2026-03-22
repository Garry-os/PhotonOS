// 
// FS syscalls like open(), close(), etc.
//
#include <user/syscalls.h>
#include <task.h>
#include <vfs/vfs.h>
#include <qemu/print.h>

#define SYS_WRITE 1
ssize_t sys_write(int fd, const char* buffer, size_t count)
{
	if (count == 0)
	{
		return 0;
	}

	fileHandle* handle = fsGetFd(currentTask, fd);

	if (!handle)
	{
		return -EBADF;
	}

	return fsWrite(handle, count, buffer);
}

#define SYS_OPEN 2
ssize_t sys_open(const char* filename, int flags, int mode)
{
	// TODO: Implement VFS open() with flags and mode
	(void)flags;
	(void)mode;

	fileHandle* handle = NULL;
	ssize_t result = fsOpen(filename, &handle);
	if (RET_ERR(result))
	{
		return result;
	}

	return result;
}

void SyscallRegisterFs()
{
	registerSyscall(SYS_WRITE, sys_write);
	registerSyscall(SYS_OPEN, sys_open);
}

