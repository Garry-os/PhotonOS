#include "isr.h"
#include <qemu/print.h>
#include <x86_64/idt.h>
#include <stddef.h>
#include <console.h>
#include <arch/x86_64/pic.h>

ISRHandler g_Handlers[256];

// A list of all CPU exceptions
static const char* const g_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

// Get call from isr.asm
void interrupt_handler(cpu_registers_t* context)
{
	if (g_Handlers[context->interrupt_number] != NULL)
	{
		g_Handlers[context->interrupt_number](context);
	}
	else if (context->interrupt_number >= 32)
	{
		// Probably an unhandled interrupt
		dbg_printf("[ISR] Unhandled interrupt: 0x%x\n", context->interrupt_number);
	}
	else if (context->interrupt_number == 14)
	{
		// Page fault
		uint64_t pageFault_addr;
		asm volatile("movq %%cr2, %0" : "=r"(pageFault_addr));
		dbg_printf("[ISR] CPU triggered an exception: %s, error code: %d, page fault address: 0x%llx\n", g_Exceptions[context->interrupt_number], context->error_code, pageFault_addr);
		printf("CPU triggered an exception: %s\n", g_Exceptions[context->interrupt_number]);
		panic();
	}
	else
	{
		// Else, it is a CPU exception
		dbg_printf("[ISR] CPU triggered an exception: %s, error code: %d\n", g_Exceptions[context->interrupt_number], context->error_code);
		printf("CPU triggered an exception: %s\n", g_Exceptions[context->interrupt_number]);
		panic();
	}
}

void InitISR()
{
	RemapPIC(0x20, 0x20 + 8);
	for (int i = 0; i < 48; i++)
	{
		IDT_SetGate(i, (uint64_t)isr_stub_table[i], IDT_INTERRUPT_GATE);
	}

	InitIDT();

	// Re-enable interrupts
	asm volatile ("sti");

	(void)x86_inb(0x60);
}

void ISR_RegisterHandler(int interrupt, ISRHandler handler)
{
	g_Handlers[interrupt] = handler;
}

