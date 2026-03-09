//
// Linux-like block device
//
#include "block.h"
#include <utils/llist.h>
#include <storage/mbr.h>
#include <lock.h>

blockDevice* firstBlock;

void blockRegister(blockDevice* block)
{
	lockAcquire();
	LL_Add((void**)&firstBlock, block);
	lockRelease();

	if (block->type == DEV_TYPE_DISK)
	{
		MBR_Parse(block);
	}
}

