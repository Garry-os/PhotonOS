//
// CPU operations implementation in C using built-in assembly
//
#include "cpu.h"
#include <qemu/print.h>
#include <console.h>

void x86_outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" : : "dN"(port), "a"(value));
}

uint8_t x86_inb(uint16_t port)
{
    uint8_t result;
    asm volatile ("inb %1, %0" : "=a"(result) : "dN"(port));
    return result;
}

void panic()
{
	if (consoleInit)
	{
		clearScreen();
		printf("FATAL: Kernel panic!\n");
	}

	dbg_printf("Kernel panic!\n");
	asm volatile ("cli");
	while (1)
	{
		asm volatile ("hlt");
	}
}

void invalidate(uint64_t virtAddr)
{
	asm volatile("invlpg %0" ::"m"(virtAddr));
}

void halt()
{
	while (1)
	{
		asm volatile ("hlt");
	}
}

