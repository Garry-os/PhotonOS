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
#include <qemu/print.h>
#include <pmm.h>
#include <vmm.h>
#include <paging.h>

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

	// VMM mapping test
	vmm_MapPage((void*)0x100000000, (void*)0x8000, PF_PRESENT);

	uint64_t* test = (uint64_t*)0x100000000;
	*test = 0x10;

	dbg_printf("Test: 0x%llx\n", *test);

	printf("Hello World! 0x%x\n", 0x123);
	printf("Test\n");

	halt();
}

