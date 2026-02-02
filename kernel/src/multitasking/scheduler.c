#include "scheduler.h"
#include <task.h>
#include <utils/memory.h>
#include <x86_64/pic.h>
#include <qemu/print.h>

// In isr.asm
extern void asm_switchTask(uint64_t iretRsp);

void schedule(cpu_registers_t* context)
{
	if (!taskInitialized)
	{
		return;
	}

	currentTask->status = TASK_STATE_READY;

	task_t* next = currentTask->next;
	if (!next)
	{
		// We've been through a full loop
		next = firstTask;
	}

	while (next->status != TASK_STATE_READY)
	{
		next = next->next;

		if (!next)
		{
			// Switch to dummy task
			next = dummyTask;
			break;
		}
	}

	task_t* old = currentTask;

	next->status = TASK_STATE_RUNNING;
	currentTask = next;

	// Save CPU registers
	memcpy(&old->context, context, sizeof(cpu_registers_t));

	// Setup the new CPU context in the task's stack
	cpu_registers_t* iretqRsp = (cpu_registers_t*)(next->context.rsp - sizeof(cpu_registers_t));
	memcpy(iretqRsp, &next->context, sizeof(cpu_registers_t));

	PIC_SendEOI(context->interrupt_number);
	// Switch task in asm
	asm_switchTask((uint64_t)iretqRsp);
}

