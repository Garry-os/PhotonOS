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

	fileHandle* handle = currentTask->fds[fd];

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

	// TODO: VFS should do this!!!!
	int fd = 0;
	for (int i = 0; i < 32; i++)
	{
		if (currentTask->fds[i] == 0)
		{
			fd = i;
			break;
		}
	}

	fileHandle* handle = NULL;
	ssize_t result = fsOpen(filename, &handle);
	if (result != 0)
	{
		return result;
	}

	currentTask->fds[fd] = handle;
	return fd;
}

void SyscallRegisterFs()
{
	registerSyscall(SYS_WRITE, sys_write);
	registerSyscall(SYS_OPEN, sys_open);
}

