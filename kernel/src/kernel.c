//
// Kernel entry point
//

#include <boot.h>
#include <x86_64/cpu.h>
#include <x86_64/gdt.h>
#include <x86_64/idt.h>
#include <x86_64/isr.h>
#include <x86_64/irq.h>
#include <x86_64/timer.h>
#include <fb.h>
#include <console.h>
#include <qemu/print.h>
#include <pmm.h>
#include <vmm.h>
#include <paging.h>
#include <malloc.h>
#include <ps2/keyboard.h>

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

	// Initialize architecture-related stuff
	InitGDT();
	InitIDT();
	InitISR();

	// Initialize memory management
	InitPMM();
	InitVMM();
	InitHeap();

	InitIRQ();
	InitPIT(1193);

	InitPS2Keyboard();

	printf("Hello World! 0x%x\n", 0x123);
	printf("Test\n");

	while (1)
	{
		char c = getKey();
		putc(c);
	}

	halt();
}

