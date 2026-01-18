#include "fb.h"
#include <stddef.h>
#include <qemu/debug.h>
#include <x86_64/system.h>

Fb_Info* g_Info = NULL;
uint32_t Fb_Width = 0;
uint32_t Fb_Height = 0;

bool FbInitialized = false;

void InitFb(Fb_Info* info)
{
	if (!info)
	{
		dbg_printf("Invalid framebuffer info pointer!\n");
		kpanic();
	}

	g_Info = info;
	Fb_Width = info->width;
	Fb_Height = info->height;

	FbInitialized = true;
}

void Fb_PlotPixel(uint32_t x, uint32_t y, uint32_t color)
{
	*((uint32_t*)(g_Info->base + 4 * g_Info->pixelsPerScanline * y + 4 * x)) = color;
}

// Clear the entire screen
void Fb_Clear(uint32_t color)
{
	for (uint32_t y = 0; y < Fb_Height; y++)
	{
		for (uint32_t x = 0; x < Fb_Width; x++)
		{
			Fb_PlotPixel(x, y, color);
		}
	}
}

void Fb_DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
	for (uint32_t yy = 0; yy < h; yy++)
	{
		for (uint32_t xx = 0; xx < w; xx++)
		{
			Fb_PlotPixel(x + xx, y + yy, color);
		}
	}
}



