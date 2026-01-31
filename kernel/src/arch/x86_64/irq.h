#pragma once
#include <x86_64/cpu.h>

void InitIRQ();

typedef void (*IRQHandler)(cpu_registers_t* context);

void IRQ_RegisterHandler(int irq, IRQHandler handler);

