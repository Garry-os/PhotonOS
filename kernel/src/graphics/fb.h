#pragma once
#include <stdint.h>

typedef struct
{
	uint64_t width;
	uint64_t height;
	uint64_t pitch;
} Fb_Info;

extern Fb_Info g_FbInfo;

void InitFb();

