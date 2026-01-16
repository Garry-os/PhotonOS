//
// Kernel starting entry file
//
#include <x86_64/system.h>
#include <qemu/debug.h>

void main()
{
	dbg_printf("Hello World from kernel!\n");
	dbg_printf("0x%x\n", 0x123);
	while (1);
}

