#include "irq.h"
#include <x86_64/pic.h>
#include <x86_64/isr.h>
#include <qemu/print.h>
#include <stddef.h>

#define PIC_REMAP_OFFSET 0x20

IRQHandler g_IRQHandlers[16];

void IRQ_Handler(cpu_registers_t* context)
{
	int irq = context->interrupt_number - PIC_REMAP_OFFSET;

	if (g_IRQHandlers[irq] != NULL)
	{
		g_IRQHandlers[irq](context);
	}
	else
	{
		dbg_printf("Unhandled IRQ: %d\n", irq);
	}

	PIC_SendEOI(context->interrupt_number);
}

void InitIRQ()
{
	// Already remapped

	for (int i = 0; i < 16; i++)
	{
		ISR_RegisterHandler(PIC_REMAP_OFFSET + i, IRQ_Handler);
	}
}

void IRQ_RegisterHandler(int irq, IRQHandler handler)
{
	g_IRQHandlers[irq] = handler;
}

