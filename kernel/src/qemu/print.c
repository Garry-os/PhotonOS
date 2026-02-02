#include "print.h"
#include <x86_64/cpu.h>
#include <utils/printf.h>
#include <lock.h>

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

void dbg_printf(const char* fmt, ...)
{
	lockAcquire();
	va_list args;
	va_start(args, fmt);

	printf_internal(dbg_putc, fmt, args);

	va_end(args);

	lockRelease();
}

