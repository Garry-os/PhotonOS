#include "system.h"

void x86_outb(uint16_t port, uint8_t value)
{
	asm volatile ("outb %1, %0" : : "dN"(port), "a"(value));	
}

uint8_t x86_inb(uint16_t port)
{
	uint8_t result;
	asm volatile ("inb %1, %0" : "=a"(result) : "dN"((port)));
	return result;
}

// Kernel panic!
void kpanic()
{
	asm volatile ("cli"); // Disable interrupts
	while (1)
	{
		asm volatile ("hlt");
	}
}

// Halt using a loop
// Can be escaped by an interrupt
void khalt()
{
	while (1)
	{
		asm volatile ("hlt");
	}
}

