//
// devtmpfs VFS interface
// Responsible for registering VFS interace
//

#include "dev.h"

fs_ops_t devOps = {
	.open = devOpen,
	.close = devClose,
	.write = devWrite,
	.read = devRead,
	.getFileSize = 0,
	.seek = 0,
	.readdir = 0
};

void devRegisterVfs(mountpoint_t* mnt)
{
	mnt->ops = &devOps;
}


