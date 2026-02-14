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

void x86_outl(uint16_t port, uint32_t value)
{
    asm volatile ("outl %1, %0" : : "d"(port), "a"(value));
}

uint32_t x86_inl(uint16_t port)
{
    uint32_t result;
    asm volatile ("inl %1, %0" : "=a"(result) : "d"(port));
    return result;
}

void panic()
{
	if (consoleInit)
	{
		printf("FATAL: Kernel panic!\n");
	}

	dbg_printf("Kernel panic!\n");
	asm volatile ("cli");
	while (1)
	{
		asm volatile ("hlt");
	}
}

void x86_iowait()
{
	x86_outb(0x80, 0);
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

