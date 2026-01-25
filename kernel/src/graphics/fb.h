#pragma once
#include <stdint.h>

#define RGB(r, g, b) ((r << 16) | (g << 8) | (b))

typedef struct
{
	uint64_t width;
	uint64_t height;
	uint64_t pitch;
} Fb_Info;

extern Fb_Info g_FbInfo;

void InitFb();

void PlotPixel(uint32_t x, uint32_t y, uint32_t color);
void DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void FbClear(uint32_t color);

