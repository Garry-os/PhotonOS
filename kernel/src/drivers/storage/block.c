//
// Linux-like block device
//
#include "block.h"
#include <utils/llist.h>
#include <storage/mbr.h>
#include <lock.h>

blockDevice* firstBlock;
lock_t blockLock;

void blockRegister(blockDevice* block)
{
	lockAcquire(&blockLock);
	LL_Add((void**)&firstBlock, block);
	lockRelease(&blockLock);

	if (block->type == DEV_TYPE_DISK)
	{
		MBR_Parse(block);
	}
}

