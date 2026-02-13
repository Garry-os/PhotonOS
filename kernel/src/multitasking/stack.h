#pragma once
#include <task.h>

#define STACK_SIZE (1024 * 16) // 16 KB
#define STACK_TOP_ADDRESS ((void*)0x800000000000) // Location doesn't really matter

void* AllocateStack();
void MapStack(task_t* task);

