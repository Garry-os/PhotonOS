//
// QEMU 0xE9 port printing
//
#include "debug.h"
#include <x86_64/system.h>
#include <utils/printf.h>

#define SERIAL_PORT 0xE9

void dbg_putc(char c)
{
	x86_outb(SERIAL_PORT, c);
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
	va_list args;
	va_start(args, fmt);

	printf_internal(dbg_putc, fmt, args);

	va_end(args);

}

