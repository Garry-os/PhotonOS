//
// Core kernel's syscalls and
// Kernel's syscall interrupt handling
//
#include "syscalls.h"
#include <x86_64/isr.h>
#include <qemu/print.h>

// Syscall int 0x80 handler
void syscallHandler(cpu_registers_t* cpu)
{
	dbg_printf("[Syscall] Syscall fired, rax=0x%x\n", cpu->rax);
}

void InitSyscall()
{
	// Register the syscall handler
	ISR_RegisterHandler(0x80, syscallHandler);

	dbg_printf("[Syscall] All syscalls are registered.\n");
}

