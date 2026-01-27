#pragma once
#include <stdint.h>

#define IDT_INTERRUPT_GATE 0x8E
#define IDT_TRAP_GATE 0x8F

typedef struct
{
	uint16_t OffsetLow; // bit 0 - 15
	uint16_t Selector; // bit 16 - 31
	uint8_t ist;        // bit 32 - 34 (35 - 39 is reserved)
	uint8_t Attributes; // bit 40 - 48
	uint16_t OffsetMid; // bit 48 - 63
	uint32_t OffsetHigh; // bit 64 - 95
	uint32_t Reserved; // Reserved (bit 96 - 127)
} __attribute__((packed)) IDTDescriptor;

// Similar the the GDTR
typedef struct
{
	uint16_t Limit; // Size
	uint64_t Base; // Address
} __attribute__((packed)) IDTR;

void IDT_SetGate(int interrupt, uint64_t handler, uint8_t flags);
void InitIDT();

