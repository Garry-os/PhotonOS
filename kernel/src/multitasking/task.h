#pragma once
#include <stddef.h>
#include <x86_64/cpu.h>
#include <stdbool.h>
#include <stdint.h>
#include <vfs/vfs.h>

#define KERNEL_TASK_ID 0
#define MAX_TASK_FDS 32

extern bool taskInitialized;
extern bool schedulerReady;

typedef enum
{
	TASK_STATE_RUNNING,
	TASK_STATE_READY,
	TASK_STATE_CREATED,
	TASK_STATE_DUMMY
} taskState;

// Linked list
typedef struct task
{
	struct task* next;

	// General task's informations
	size_t id; // Task ID
	uint8_t status;

	// CPU-specific task's informations
	cpu_registers_t context;
	uint64_t* pd;
	uint64_t rsp0;
	bool isKernelTask;

	// Unix
	fileHandle* fds[MAX_TASK_FDS];
} task_t;

extern task_t* firstTask;
extern task_t* currentTask;
extern task_t* dummyTask;

task_t* TaskCreate(uint64_t entry, uint64_t* pd, bool isKernelTask);

task_t* TaskGet(size_t id);

void yield();

void InitTasks();

