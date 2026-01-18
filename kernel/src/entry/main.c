//
// Kernel starting entry file
//
#include <x86_64/system.h>
#include <qemu/debug.h>
#include <osloader.h>
#include <fb.h>
#include <utils/color.h>

void main()
{
	BootInfo* bootInfo = (BootInfo*)BOOTINFO_ADDRESS;
	if (bootInfo->magic != BOOT_MAGIC)
	{
		dbg_printf("Invalid boot magic!\n");
		kpanic();
	}

	dbg_printf("Hello World from kernel!\n");
	dbg_printf("0x%x\n", 0x123);

	dbg_printf("Boot info magic: 0x%llx\n", bootInfo->magic);

	// Init the framebuffer
	InitFb(bootInfo->fbInfo);

	// Clear screen (color black)
	Fb_Clear(RGB(0, 0, 0));

	Fb_DrawRect(100, 100, 50, 50, RGB(255, 10, 20));

	khalt();
}

