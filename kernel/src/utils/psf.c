#include "psf.h"
#include <utils/gohufont.h>
#include <qemu/print.h>
#include <fb.h>

PSFHeader* g_psf;

bool PsfLoad(void* buffer)
{
	PSFHeader* header = (PSFHeader*)buffer;

	// Check magic
	if (header->magic != PSF_MAGIC)
	{
		dbg_printf("[PSF] Invalid PSF1 magic!\n");
		return false;
	}

	g_psf = (PSFHeader*)buffer;
	return true;
}

bool PsfLoadDefaultFont()
{
	return PsfLoad(g_gohufont);
}

void PsfDrawChar(uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, char c)
{
	uint8_t* offset = (uint8_t*)((uint64_t)g_psf + sizeof(PSFHeader) + c * g_psf->height);

	for (int i = 0; i < g_psf->height; i++)
	{
		for (int j = 0; j < PSF_WIDTH; j++)
		{
			if (offset[i] & (1 << (8 - j))) // Big endian
			{
				PlotPixel(x + j, y + i, fg);
			}
			else
			{
				PlotPixel(x + j, y + i, bg);
			}
		}
	}
}

