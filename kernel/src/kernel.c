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
#include <malloc.h>

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

	// Allocation test
	void* address = malloc(0x100);
	void* address1 = malloc(0x100);
	void* address2 = malloc(0x100);
	dbg_printf("Malloc result: 0x%llx\n", address);
	dbg_printf("Malloc result: 0x%llx\n", address1);
	dbg_printf("Malloc result: 0x%llx\n", address2);
	free(address2);

	// After freeing
	dbg_printf("After freeing\n");
	dbg_printf("Malloc result: 0x%llx\n", address);
	dbg_printf("Malloc result: 0x%llx\n", malloc(0x100));


	printf("Hello World! 0x%x\n", 0x123);
	printf("Test\n");

	halt();
}

