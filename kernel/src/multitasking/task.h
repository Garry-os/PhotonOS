#pragma once
#include <stddef.h>
#include <x86_64/cpu.h>
#include <stdbool.h>
#include <stdint.h>

#define KERNEL_TASK_ID 0

extern bool taskInitialized;
extern bool schedulerReady;

typedef enum
{
	TASK_STATE_RUNNING,
	TASK_STATE_READY,
	TASK_STATE_DUMMY
} taskState;

// Linked list
typedef struct task
{
	size_t id; // Task ID
	cpu_registers_t context;
	struct task* next;
	uint8_t status;
	uint64_t* pd;

	uint64_t iretqRsp; // TODO: Implement TSS instead of this
} task_t;

extern task_t* firstTask;
extern task_t* currentTask;
extern task_t* dummyTask;

task_t* TaskCreate(void (*entry)(void), uint64_t* pd);

task_t* TaskGet(size_t id);

void InitTasks();

