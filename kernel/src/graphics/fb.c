#include "fb.h"
#include <limine.h>
#include <x86_64/cpu.h>
#include <qemu/print.h>
#include <stddef.h>

uint32_t* fbBase = NULL;

Fb_Info g_FbInfo;

static volatile struct limine_framebuffer_request limineFbReq = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

void InitFb()
{
	struct limine_framebuffer_response* fbRes = limineFbReq.response;
	// Check if framebuffer is avaliable
	if (fbRes == NULL || fbRes->framebuffer_count < 1)
	{
		dbg_printf("[Framebuffer] Failed to request a framebuffer!\n");
		panic();
	}

	// Copy framebuffer's info
	struct limine_framebuffer* fb = fbRes->framebuffers[0];
	g_FbInfo.width = fb->width;
	g_FbInfo.height = fb->height;
	g_FbInfo.pitch = fb->pitch;
	fbBase = (uint32_t*)fb->address;

	// width x height x pitch
	dbg_printf("[Framebuffer] resolution: %dx%dx%d\n", g_FbInfo.width, g_FbInfo.height, g_FbInfo.pitch);
}

void PlotPixel(uint32_t x, uint32_t y, uint32_t color)
{
	uint32_t offset = (y * g_FbInfo.pitch) + (x * 4);
	fbBase[offset / 4] = color;
}

void DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
	for (uint32_t yy = 0; yy < h; yy++)
	{
		for (uint32_t xx = 0; xx < w; xx++)
		{
			PlotPixel(xx + x, yy + y, color);
		}
	}
}

void FbClear(uint32_t color)
{
	for (uint32_t y = 0; y < g_FbInfo.height; y++)
	{
		for (uint32_t x = 0; x < g_FbInfo.width; x++)
		{
			PlotPixel(x, y, color);
		}
	}
}

