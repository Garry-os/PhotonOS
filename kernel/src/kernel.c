//
// Kernel entry point
//

#include <limine.h>
#include <boot.h>
#include <qemu/print.h>

// Set limine base revision to 4
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

void halt()
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

	dbg_puts("Hello World!\n");

	halt();
}

