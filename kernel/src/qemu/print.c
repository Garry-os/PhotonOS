#include "print.h"
#include <x86_64/cpu.h>

#define QEMU_SERIAL_PORT 0xE9

void dbg_putc(char c)
{
	x86_outb(QEMU_SERIAL_PORT, c);
}

void dbg_puts(const char* str)
{
	while (*str)
	{
		dbg_putc(*str);
		str++;
	}
}

