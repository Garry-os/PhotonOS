#include "stack.h"
#include <malloc.h>
#include <pmm.h>
#include <vmm.h>
#include <utils/memory.h>
#include <paging.h>
#include <qemu/print.h>
#include <utils/memory.h>

void* AllocateStack()
{
	void* bottom = vmm_Allocate(STACK_SIZE / PAGE_SIZE);
	void* top = (void*)((size_t)top + STACK_SIZE);
	memset(bottom, 0, STACK_SIZE);

	return top;
}

void MapStack(task_t* task)
{
	// Switch to the task's pd temporarily
	uint64_t* oldPd = vmm_GetCurrentPd();
	vmm_SwitchPd(task->pd);

	for (size_t i = 0; i < STACK_SIZE / PAGE_SIZE; i++)
	{
		size_t virtAddr = (size_t)STACK_TOP_ADDRESS - STACK_SIZE + i * PAGE_SIZE;
		vmm_MapPage((void*)virtAddr, pmm_Allocate(1), PF_USER);
		memset((void*)virtAddr, 0, PAGE_SIZE);
	}

	vmm_SwitchPd(oldPd);
}

