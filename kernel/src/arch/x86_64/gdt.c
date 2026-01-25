#include "gdt.h"
#include <stdint.h>

typedef struct
{
	uint16_t Limit; // bit 0 - 15
	uint16_t BaseLow; // bit 16 - 31
	uint8_t BaseMid; // bit 32 - 39
	uint8_t Access; // bit 40 - 47
	uint8_t Flags; // Including limit at bit 48 - 51, flags bit 52 - 55
	uint8_t BaseHigh; // bit 56 - 63
} __attribute__((packed)) GDTEntry;

typedef struct
{
	uint16_t Limit; // Size
	uint64_t Base; // Address
} __attribute__((packed)) GDTR;

typedef struct
{
	GDTEntry entries[3];
} __attribute__((packed)) GDTEntries;

GDTEntries gdt;
GDTR gdtr;

// gdt.asm
extern void FlushGDT(GDTR* gdtr);

void InitGDT()
{
	// NULL descriptor
	gdt.entries[0].Limit = 0;
	gdt.entries[1].BaseLow = 0;
	gdt.entries[1].BaseMid = 0;
	gdt.entries[1].Access = 0;
	gdt.entries[1].Flags = 0;
	gdt.entries[1].BaseHigh = 0;

	// Kernel 64 bit code segment (offset 0x08)
	gdt.entries[1].Limit = 0;
	gdt.entries[1].BaseLow = 0;
	gdt.entries[1].BaseMid = 0;
	gdt.entries[1].Access = 0b10011010; // Read-write, executable, code segment, present
	gdt.entries[1].Flags = 0b00100000; // Long mode code segment
	gdt.entries[1].BaseHigh = 0;

	// Kernel 64 bit data segment (offset 0x10)
	gdt.entries[2].Limit = 0;
	gdt.entries[2].BaseLow = 0;
	gdt.entries[2].BaseMid = 0;
	gdt.entries[2].Access = 0b10010010; // Read-write, data segment, present
	gdt.entries[2].Flags = 0;
	gdt.entries[2].BaseHigh = 0;

	// Construct GDTR
	gdtr.Limit = sizeof(GDTEntries) - 1;
	gdtr.Base = (uint64_t)&gdt;

	FlushGDT(&gdtr);
}

