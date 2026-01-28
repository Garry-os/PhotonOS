//
// Kernel entry point
//

#include <boot.h>
#include <x86_64/cpu.h>
#include <x86_64/gdt.h>
#include <x86_64/idt.h>
#include <x86_64/isr.h>
#include <fb.h>
#include <console.h>
#include <utils/bitmap.h>
#include <qemu/print.h>

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
	InitConsole();
	clearScreen();

	// Initialized architecture-related stuff
	InitGDT();
	InitIDT();
	InitISR();

	// Bitmap test
	uint8_t buffer[10] = { 0 };
	bitmap_t bitmap;
	bitmap.buffer = &buffer[0];
	bitmap.size = sizeof(buffer);

	BitmapSet(&bitmap, 0, true);
	BitmapSet(&bitmap, 2, true);
	BitmapSet(&bitmap, 4, true);
	BitmapSet(&bitmap, 6, true);
	BitmapSet(&bitmap, 8, true);

	for (int i = 0; i < 10; i++)
	{
		dbg_printf("%d: %d\n", i, BitmapGet(&bitmap, i));
	}

	printf("Hello World! 0x%x\n", 0x123);
	printf("Test\n");

	halt();
}

