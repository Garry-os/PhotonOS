#pragma once
#include <osloader.h>
#include <stdbool.h>

// Has been initialized or not?
extern bool fbInitialized;

void InitFb(Fb_Info* info);

// Framebuffer drawing functions
void Fb_PlotPixel(uint32_t x, uint32_t y, uint32_t color);
void Fb_Clear(uint32_t color);
void Fb_DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

