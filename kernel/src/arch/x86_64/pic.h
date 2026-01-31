#pragma once
#include <stdint.h>

void RemapPIC(uint8_t offset1, uint8_t offset2);
void PIC_SendEOI(int irq);

