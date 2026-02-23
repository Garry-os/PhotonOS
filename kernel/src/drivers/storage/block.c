//
// Linux-like block device
//
#include "block.h"
#include <utils/llist.h>
#include <storage/mbr.h>

blockDevice* firstBlock;

void blockRegister(blockDevice* block)
{
	LL_Add((void**)&firstBlock, block);

	if (block->type == DEV_TYPE_DISK)
	{
		MBR_Parse(block);
	}
}

