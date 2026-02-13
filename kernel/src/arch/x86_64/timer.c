//
// PIT timer driver implementation
//
#include "timer.h"
#include <x86_64/cpu.h>
#include <x86_64/irq.h>
#include <scheduler.h>
#include <console.h>

#define TIMER_ACCURANCY 1193182 // 1.193182 MHz

uint32_t timerFreq;
uint64_t ticks;

void InitPIT(uint32_t count)
{
	timerFreq = TIMER_ACCURANCY / count;

	// bit 1 - 3: Square wave generator (mode 3) (011)
	// With channel 0
	x86_outb(0x43, 0b00110110);

	uint8_t low = (uint8_t)(timerFreq & 0xFF);
	uint8_t high = (uint8_t)(timerFreq >> 8 && 0xFF);

	x86_outb(0x40, low);
	x86_outb(0x40, high);

	ticks = 0;

	IRQ_RegisterHandler(0, timerTicks);
}

void timerTicks(cpu_registers_t* context)
{
	ticks++;

	schedule(context);
}

void sleep(uint64_t miliseconds)
{
	uint64_t start = ticks;
	while (ticks < start + miliseconds)
	{
		asm volatile ("hlt");
	}
}

