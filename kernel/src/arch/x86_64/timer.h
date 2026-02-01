#pragma once
#include <stdint.h>
#include <x86_64/cpu.h>

extern uint64_t ticks;

void InitPIT(uint32_t count);

void timerTicks(cpu_registers_t* context);
void sleep(uint64_t miliseconds);

