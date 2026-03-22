//
// Core kernel's syscalls and
// Kernel's syscall interrupt handling
//
#include "syscalls.h"
#include <x86_64/isr.h>
#include <qemu/print.h>
#include <stddef.h>
#include <x86_64/cpu.h>

#define MAX_SYSCALLS 411
static uint64_t syscallsHandler[MAX_SYSCALLS];
static int syscallsCnt = 0;

// Syscalls arguments (6 arguments)
typedef ssize_t (*SyscallArgs)(uint64_t arg1, uint64_t arg2, uint64_t arg3, 
		uint64_t arg4, uint64_t arg5, uint64_t arg6);
// Syscall int 0x80 handler
void syscallHandler(cpu_registers_t* cpu)
{
	int syscallNum = cpu->rax;
	if (syscallsHandler[syscallNum] != 0)
	{
		// Evoke the syscall
		uint64_t ret = ((SyscallArgs)(syscallsHandler[syscallNum]))(cpu->rdi, cpu->rsi, cpu->rdx, 
			cpu->r10, cpu->r8, cpu->r9);
		cpu->rax = ret;
	}
}

void registerSyscall(int num, void* entry)
{
	// Check for limits
	if (num > MAX_SYSCALLS)
	{
		dbg_printf("[Syscall] Tried to register syscall with ID %d, max: %d\n", num, MAX_SYSCALLS);
		panic();
	}

	// Check for avalibility
	if (syscallsHandler[num] != NULL)
	{
		dbg_printf("[Syscall] %d is already occupied.\n", num);
		panic();
	}

	// Register
	syscallsHandler[num] = (uint64_t)entry;
	syscallsCnt++;
}

void InitSyscall()
{
	// Register the syscall handler
	ISR_RegisterHandler(0x80, syscallHandler);

	// Register syscalls
	// FS
	SyscallRegisterFs();

	dbg_printf("[Syscall] All syscalls are registered.\n");
}



