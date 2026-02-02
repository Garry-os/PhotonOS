#pragma once
#include <stddef.h>
#include <x86_64/cpu.h>
#include <stdbool.h>
#include <stdint.h>

#define KERNEL_TASK_ID 0

extern bool taskInitialized;

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
} task_t;

extern task_t* firstTask;
extern task_t* currentTask;
extern task_t* dummyTask;

task_t* TaskCreate(void (*entry)(void));

void InitTasks();

