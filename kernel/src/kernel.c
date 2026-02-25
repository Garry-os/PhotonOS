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
#include <task.h>
#include <pci/pci.h>
#include <storage/block.h>
#include <fat32/fat32.h>

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

	// Initialize memory management
	InitPMM();
	InitVMM();
	InitHeap();

	// Initialize architecture-related stuff
	InitGDT();
	InitISR();

	InitIRQ();
	InitPIT(1193);

	InitPS2Keyboard();
	
	// Multitasking
	InitTasks();

	InitPCI();

	printf("PhotonOS v0.0.1\n");

	PCIDevice* current = firstPCIDevice;
	while (current)
	{
		printf("Found PCI Device, vendor ID: %x, device ID: %x\n", current->vendorId, current->deviceId);

		current = current->next;
	}

	// Reading test
	// Find first block device
	blockDevice* target = firstBlock;

	uint8_t buffer[512];

	while (target)
	{
		dbg_printf("Block name: %s, model: %s\n", target->name, target->modelName);

		// Mount the partition
		if (target->type == DEV_TYPE_PARTITION)
		{
			fat32_mount(target);
		}
		target = target->next;
	}

	// Read a file
	fat32Handle* file = fat32_open("/hello.txt");
	fat32_read(file, buffer);
	// Print out content
	dbg_printf("Content: %s\n", buffer);

	while (1)
	{
		char c = getKey();
		putc(c);
	}

	halt();
}

