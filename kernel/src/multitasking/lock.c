//
// IRQ scheduler lock
// to prevent race conditions
//
#include "lock.h"
#include <stdint.h>

void lockAcquire(lock_t* lock)
{
	// Get current interrupt state
	uint64_t rflags = 0;
	asm volatile ("pushfq; pop %0" : "=r"(rflags));
	*lock = (rflags & (1 << 9)) != 0;

	// Disable IRQ
	asm volatile ("cli");
}

void lockRelease(lock_t* lock)
{
	// Check for interrupt flag
	if (!lock)
	{
		asm volatile ("sti");
	}
}

