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

