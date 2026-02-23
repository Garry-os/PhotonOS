//
// Linux-like block device
//
#include "block.h"
#include <utils/llist.h>

blockDevice* firstBlock;

void blockRegister(blockDevice* block)
{
	LL_Add((void**)&firstBlock, block);
}

