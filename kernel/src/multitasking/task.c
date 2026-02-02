#include "task.h"
#include <malloc.h>
#include <utils/memory.h>
#include <stack.h>

task_t* firstTask;
task_t* currentTask;
task_t* dummyTask;

bool taskInitialized = false;

// Avoid kernel task id
size_t freeId = 1;

task_t* TaskCreate(void (*entry)(void))
{
	// TODO: Implement scheduler locks
	task_t* task = (task_t*)malloc(sizeof(task_t));
	memset(task, 0, sizeof(task_t));

	asm volatile ("cli");

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
	task->context.ss = 0x10; // Kernel DS
	task->context.cs = 0x08; // Kernel CS
	task->context.rsp = (uint64_t)AllocateStack();
	task->context.rflags = 0x200; // Interrupt enabled (bit 9)
	task->context.rip = (uint64_t)entry;
	task->context.rbp = 0;

	task->status = TASK_STATE_READY;

	asm volatile ("sti");

	return task;
}

// Setup the kernel task that we're currently running
void SetupKernelTask()
{
	firstTask = (task_t*)malloc(sizeof(task_t));
	memset(firstTask, 0, sizeof(task_t));

	currentTask = firstTask;
	currentTask->id = KERNEL_TASK_ID;
	currentTask->status = TASK_STATE_RUNNING;
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

	// Setup a dummy task
	dummyTask = TaskCreate(dummyTaskEntry);
	dummyTask->status = TASK_STATE_DUMMY;

	taskInitialized = true;
}


