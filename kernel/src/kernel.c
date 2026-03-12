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
#include <utils/memory.h>
#include <syscalls.h>

// Set limine base revision to 4
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

void taskEntry()
{
	while (1);
}

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

	// Enumerate over block devices
	blockDevice* target = firstBlock;

	while (target)
	{
		dbg_printf("Block name: %s, model: %s\n", target->name, target->modelName);

		// Mount the partition
		if (target->type == DEV_TYPE_PARTITION)
		{
			// Mount partition
			fsMount("/", target);
			break;
		}
		target = target->next;
	}


	// Read directory
	fileHandle* handle = fsOpen("/");
	dirent64 dir;
	while (fsReaddir(handle, &dir))
	{
		dbg_printf("%s\n", dir.name);
	}

	fsClose(handle);

	InitSyscall();

	task_t* task = TaskCreate(taskEntry, vmm_CopyKernelPd());
	task->status = TASK_STATE_READY;

	while (1)
	{
		char c = getKey();
		putc(c);
	}

	halt();
}

