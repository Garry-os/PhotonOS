#pragma once
#include <x86_64/cpu.h>

// ISR stub table from isr.asm
extern void* isr_stub_table[];

typedef void (*ISRHandler)(cpu_registers_t* context);

void InitISR();
void ISR_RegisterHandler(int interrupt, ISRHandler handler);

