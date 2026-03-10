#include "gdt.h"
#include <utils/memory.h>

typedef struct
{
	uint16_t Limit; // bit 0 - 15
	uint16_t BaseLow; // bit 16 - 31
	uint8_t BaseMid; // bit 32 - 39
	uint8_t Access; // bit 40 - 47
	uint8_t Flags; // Including limit at bit 48 - 51, flags bit 52 - 55
	uint8_t BaseHigh; // bit 56 - 63
} __attribute__((packed)) GDTEntry;

// 64 bit system segment descriptor
typedef struct
{
	uint16_t Limit; // bit 0 - 15
	uint16_t BaseLow; // bit 16 - 31
	uint8_t BaseMid; // bit 32 - 39
	uint8_t Access; // bit 40 - 47
	uint8_t Flags; // bit 48 - 55 (48 - 51 is 4-bit upper limit)
	uint8_t BaseHigh; // bit 56 - 63
	uint32_t BaseUpper; // bit 64 - 95
	uint32_t reserved; // bit 96 - 127
} __attribute__((packed)) TSSDescriptor;

typedef struct
{
	uint16_t Limit; // Size
	uint64_t Base; // Address
} __attribute__((packed)) GDTR;

typedef struct
{
	GDTEntry entries[5];
	TSSDescriptor tss;
} __attribute__((packed)) GDTEntries;

static GDTEntries gdt;
static GDTR gdtr;
static TSS tss;

TSS* tssPtr = &tss;

// gdt.asm
extern void FlushGDT(GDTR* gdtr);

void LoadTSS(TSS* ptr)
{
	uint64_t addr = (uint64_t)ptr;

	gdt.tss.BaseLow = (uint16_t)addr;
	gdt.tss.BaseMid = (uint16_t)(addr >> 16);
	gdt.tss.Access = 0b10001001; // Access, executable, present
	gdt.tss.Flags = 0;
	gdt.tss.BaseHigh = (uint8_t)(addr >> 24);
	gdt.tss.BaseUpper = (uint32_t)(addr >> 32);
	gdt.tss.reserved = 0;

	asm volatile("ltr %0" : : "rm"((uint16_t)0x28) : "memory");
}

void InitGDT()
{
	// NULL descriptor
	gdt.entries[0].Limit = 0;
	gdt.entries[0].BaseLow = 0;
	gdt.entries[0].BaseMid = 0;
	gdt.entries[0].Access = 0;
	gdt.entries[0].Flags = 0;
	gdt.entries[0].BaseHigh = 0;

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

	// User 64 bit code segment (offset 0x18)
	gdt.entries[3].Limit = 0;
	gdt.entries[3].BaseLow = 0;
	gdt.entries[3].BaseMid = 0;
	gdt.entries[3].Access = 0b11111010; // Read-write, executable, code segment, DPL = 3, present
	gdt.entries[3].Flags = 0b00100000; // Long mode code segment
	gdt.entries[3].BaseHigh = 0;

	// User 64 bit data segment (offset 0x20)
	gdt.entries[4].Limit = 0;
	gdt.entries[4].BaseLow = 0;
	gdt.entries[4].BaseMid = 0;
	gdt.entries[4].Access = 0b11110010; // Read-write, data segment, DPL = 3, present
	gdt.entries[4].Flags = 0;
	gdt.entries[4].BaseHigh = 0;

	// TSS (offset 0x28)
	// Stub TSS
	gdt.tss.Limit = 104;
	gdt.tss.BaseLow = 0;
	gdt.tss.BaseMid = 0;
	gdt.tss.Access = 0b10001001; // Access, executable, present
	gdt.tss.Flags = 0;
	gdt.tss.BaseHigh = 0;
	gdt.tss.BaseUpper = 0;
	gdt.tss.reserved = 0;

	// Construct GDTR
	gdtr.Limit = sizeof(GDTEntries) - 1;
	gdtr.Base = (uint64_t)&gdt;

	FlushGDT(&gdtr);

	memset(&tss, 0, sizeof(TSS));
	LoadTSS(&tss);
}

