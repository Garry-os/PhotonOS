//
// Kernel starting entry file
//
#include <x86_64/system.h>
#include <qemu/debug.h>

void main()
{
	dbg_puts("Hello World from kernel!\n");
	while (1);
}
