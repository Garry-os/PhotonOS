#include "task.h"
#include <malloc.h>
#include <utils/memory.h>
#include <stack.h>
#include <lock.h>
#include <vmm.h>
#include <qemu/print.h>
#include <scheduler.h>
#include <x86_64/isr.h>

task_t* firstTask;
task_t* currentTask;
task_t* dummyTask;

bool taskInitialized = false;
bool schedulerReady = false;

// Avoid kernel task id
size_t freeId = 1;

lock_t taskLock;

task_t* TaskCreate(uint64_t entry, uint64_t* pd, bool isKernelTask)
{
	task_t* task = (task_t*)malloc(sizeof(task_t));
	memset(task, 0, sizeof(task_t));

	lockAcquire(&taskLock);

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
	lockRelease(&taskLock);

	task->id = freeId++;
	task->status = TASK_STATE_CREATED;
	task->pd = pd;

	// IMPORTANT: Make sure to set the task's pd first before mapping the stack
	MapStack(task);
	
	if (isKernelTask)
	{
		task->context.ss = 0x10; // Kernel DS
		task->context.cs = 0x08; // Kernel CS
	}
	else
	{
		task->context.ss = 0x20 | 0x03; // User DS
		task->context.cs = 0x18 | 0x03; // User CS
	}

	task->context.rsp = (uint64_t)STACK_TOP_ADDRESS;
	task->context.rflags = 0x200; // Interrupt enabled (bit 9)
	task->context.rip = entry;
	task->context.rbp = 0;
	task->rsp0 = (uint64_t)AllocateStack();
	task->isKernelTask = isKernelTask;

	// Setup fds
	fsOpen("/dev/stdin", &task->fds[0]);
	fsOpen("/dev/stdout", &task->fds[1]);
	fsOpen("/dev/stderr", &task->fds[2]);

	return task;
}

task_t* TaskGet(size_t id)
{
	lockAcquire(&taskLock);

	task_t* index = firstTask;
	while (index)
	{
		if (index->id == id)
		{
			break;
		}

		index = index->next;
	}

	lockRelease(&taskLock);
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
	currentTask->rsp0 = (uint64_t)AllocateStack();
	currentTask->isKernelTask = true;
}

void dummyTaskEntry()
{
	while (1)
	{
		// asm volatile ("hlt");
		asm volatile ("pause");
	}
}

void yield()
{
	// Triggers yield interrupt
	asm volatile ("int $0x81");
}

// Initializes & create essential tasks
void InitTasks()
{
	SetupKernelTask();

	// Setup yield interrupt
	ISR_RegisterHandler(0x81, schedule);

	taskInitialized = true;

	// Setup a dummy task
	dummyTask = TaskCreate((uint64_t)dummyTaskEntry, vmm_CopyKernelPd(), true);
	dummyTask->status = TASK_STATE_DUMMY;

	schedulerReady = true;
	dbg_printf("[Task] Pre-emptive multitasking fully initialized!\n");
}


