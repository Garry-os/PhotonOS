//
// Manages /dev/std* files
//
#include <console.h>
#include <qemu/print.h>
#include <vfs/vfs.h>
#include <lock.h>

size_t stdioRead(fileHandle* handle, uint32_t limit, void* buffer)
{
	dbg_printf("[Stdio] Implement read!\n");
	return 0;
}

size_t stdioWrite(fileHandle* handle, uint32_t limit, void* buffer)
{
	// Write to the console
	char* cBuffer = (char*)buffer;

	lockAcquire();
	for (uint32_t i = 0; i < limit; i++)
	{
		putc(cBuffer[i]);
	}
	lockRelease();

	return limit;
}

fs_ops_t stdioOps = {
	.open = 0,
	.close = 0,

	.read = stdioRead,
	.write = stdioWrite,

	.getFileSize = 0,
	.seek = 0,
	.readdir = 0
};



