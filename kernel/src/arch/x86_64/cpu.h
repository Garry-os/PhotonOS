#pragma once
#include <stdint.h>

// CPU context
typedef struct
{
	// Order is reverse
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	
	uint64_t interrupt_number;
	uint64_t error_code;
	
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} cpu_registers_t;

void x86_outb(uint16_t port, uint8_t value);
uint8_t x86_inb(uint16_t port);

void x86_outl(uint16_t port, uint32_t value);
uint32_t x86_inl(uint16_t port);

void x86_iowait();

void panic();
void halt();

void invalidate(uint64_t virtAddr);

