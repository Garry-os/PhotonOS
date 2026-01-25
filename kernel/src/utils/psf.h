//
// PSF1 font loading & drawing
//
#pragma once
#include <stdint.h>

#define PSF_MAGIC 0x0436
#define PSF_WIDTH 0x08 // Width is always 8 pixels

typedef struct
{
	uint16_t magic;
	uint8_t mode;
	uint8_t height;
} PSFHeader;

extern PSFHeader* g_psf;

bool PsfLoad(void* buffer);
bool PsfLoadDefaultFont();

void PsfDrawChar(uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, char c);

