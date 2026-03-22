//
// Manages VFS file descriptors
//
#include "vfs.h"
#include <task.h>
#include <qemu/print.h>

int fsFindFreeFd(task_t* task)
{
	int fd = -1;

	for (int i = 0; i < MAX_TASK_FDS; i++)
	{
		if (!task->fds[i])
		{
			fd = i;
			break;
		}
	}

	return fd;
}

void fsRegisterFd(task_t* task, int fd, fileHandle* handle)
{
	task->fds[fd] = handle;
}

fileHandle* fsGetFd(task_t* task, int fd)
{
	return task->fds[fd];
}

