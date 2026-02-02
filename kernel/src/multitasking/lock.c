#include "lock.h"

int irq_disable_cnt = 0;

void lockAcquire()
{
	asm volatile ("cli");
	irq_disable_cnt++;
}

void lockRelease()
{
	irq_disable_cnt--;
	if (irq_disable_cnt == 0)
	{
		asm volatile ("sti");
	}
}

