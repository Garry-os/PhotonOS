//
// Kernel entry point
//

#include <limine.h>
#include <boot.h>
#include <qemu/print.h>
#include <x86_64/cpu.h>
#include <x86_64/gdt.h>
#include <fb.h>

// Set limine base revision to 4
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

void start(void)
{
	// Check if revision is supported
	if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false)
	{
		halt();
	}

	InitBootInfo();

	// Initialize graphics-related stuff
	InitFb();
	FbClear(RGB(0, 0, 0));
	DrawRect(100, 100, 50, 50, RGB(255, 10, 100));

	// Initialized architecture-related stuff
	InitGDT();

	dbg_printf("Hello World! 0x%x\n", 0x123);

	halt();
}

