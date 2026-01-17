//
// Kernel starting entry file
//
#include <x86_64/system.h>
#include <qemu/debug.h>
#include <osloader.h>

void main()
{
	BootInfo* bootInfo = (BootInfo*)0x10000;
	if (bootInfo->magic != BOOT_MAGIC)
	{
		dbg_printf("Invalid boot magic!\n");
		kpanic();
	}

	dbg_printf("Hello World from kernel!\n");
	dbg_printf("0x%x\n", 0x123);

	dbg_printf("Boot info magic: 0x%llx\n", bootInfo->magic);

	khalt();
}

