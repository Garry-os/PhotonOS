#pragma once
#include <stdint.h>

void x86_outb(uint16_t port, uint8_t value);
uint8_t x86_inb(uint16_t port);

void panic();
void halt();

