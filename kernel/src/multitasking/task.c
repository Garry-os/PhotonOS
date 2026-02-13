#include "task.h"
#include <malloc.h>
#include <utils/memory.h>
#include <stack.h>
#include <lock.h>
#include <vmm.h>
#include <qemu/print.h>

task_t* firstTask;
task_t* currentTask;
task_t* dummyTask;

bool taskInitialized = false;
bool schedulerReady = false;

// Avoid kernel task id
size_t freeId = 1;

task_t* TaskCreate(void (*entry)(void), uint64_t* pd)
{
	task_t* task = (task_t*)malloc(sizeof(task_t));
	memset(task, 0, sizeof(task_t));

	lockAcquire();

	// Find the next linked list element
	task_t* index =	firstTask;
	while (index)
	{
		if (!index->next)
		{
			break;
		}

		index = index->next;
	}

	index->next = task;

	task->id = freeId++;
	task->pd = pd;

	// IMPORTANT: Make sure to set the task's pd first before mapping the stack
	MapStack(task);
	
	task->context.ss = 0x10; // Kernel DS
	task->context.cs = 0x08; // Kernel CS
	task->context.rsp = (uint64_t)STACK_TOP_ADDRESS;
	task->context.rflags = 0x200; // Interrupt enabled (bit 9)
	task->context.rip = (uint64_t)entry;
	task->context.rbp = 0;
	task->status = TASK_STATE_READY;
	task->iretqRsp = (uint64_t)AllocateStack();

	lockRelease();

	return task;
}

task_t* TaskGet(size_t id)
{
	lockAcquire();

	task_t* index = firstTask;
	while (index)
	{
		if (index->id == id)
		{
			break;
		}

		index = index->next;
	}

	lockRelease();
	return index;
}

// Setup the kernel task that we're currently running
void SetupKernelTask()
{
	firstTask = (task_t*)malloc(sizeof(task_t));
	memset(firstTask, 0, sizeof(task_t));

	currentTask = firstTask;
	currentTask->id = KERNEL_TASK_ID;
	currentTask->status = TASK_STATE_RUNNING;
	currentTask->pd = vmm_GetCurrentPd();
	currentTask->iretqRsp = (uint64_t)AllocateStack();
}

void dummyTaskEntry()
{
	while (1)
	{
		// asm volatile ("hlt");
		asm volatile ("pause");
	}
}

// Initializes & create essential tasks
void InitTasks()
{
	SetupKernelTask();

	taskInitialized = true;

	// Setup a dummy task
	dummyTask = TaskCreate(dummyTaskEntry, vmm_CopyKernelPd());
	dummyTask->status = TASK_STATE_DUMMY;

	schedulerReady = true;
	dbg_printf("[Task] Pre-emptive multitasking fully initialized!\n");
}


