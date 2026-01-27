#include "idt.h"
#include <x86_64/cpu.h>
#include <qemu/print.h>

IDTDescriptor g_Descriptors[256]; // Max interrupts
IDTR g_idtr;

void IDT_SetGate(int interrupt, uint64_t handler, uint8_t flags)
{
	if (interrupt >= 256)
	{
		dbg_printf("[IDT] Tried to set interrupt number >= 256!\n");
		panic();
	}

	g_Descriptors[interrupt].OffsetLow = (uint16_t)handler;
	g_Descriptors[interrupt].Selector = 0x08; // Kernel 64 bit code segment
	g_Descriptors[interrupt].ist = 0;
	g_Descriptors[interrupt].Attributes = flags;
	g_Descriptors[interrupt].OffsetMid = (uint16_t)(handler >> 16);
	g_Descriptors[interrupt].OffsetHigh = (uint32_t)(handler >> 32);
	g_Descriptors[interrupt].Reserved = 0;
}

void InitIDT()
{
	// Setup the IDTR
	g_idtr.Limit = sizeof(g_Descriptors) - 1;
	g_idtr.Base = (uint64_t)&g_Descriptors;

	// Load IDT to the CPU
	asm volatile ("lidt %0" : : "m"(g_idtr) : "memory");
}

