#include "console.h"
#include <utils/psf.h>
#include <utils/printf.h>
#include <fb.h>
#include <x86_64/cpu.h>

// Cursor positions
int g_PosX = 0;
int g_PosY = 0;

uint32_t g_fg = RGB(222, 222, 222); // Foreground color
uint32_t g_bg = RGB(10, 10, 10); // Background color

void InitConsole()
{
	// Load the gohufont
	if (!PsfLoadDefaultFont())
	{
		panic();
	}
	
	g_PosX = 0;
	g_PosY = 0;
}

void clearScreen()
{
	FbClear(g_bg);

	g_PosX = 0;
	g_PosY = 0;
}

void putc(char c)
{
	switch (c)
	{
		case '\n':
			g_PosX = 0;
			g_PosY += g_psf->height;
			break;
		case '\t':
			for (int i = 0; i < 4; i++)
			{
				putc(' ');
			}
			break;
		case '\r':
			g_PosX = 0;
			break;
		default:
			PsfDrawChar(g_PosX, g_PosY, g_fg, g_bg, c);
			g_PosX += PSF_WIDTH;
			break;
	}

	if (g_PosX >= g_FbInfo.width)
	{
		g_PosX = 0;
		g_PosY += g_psf->height;
	}

	if (g_PosY >= g_FbInfo.height)
	{
		clearScreen();
	}
}

void puts(const char* str)
{
	while (*str)
	{
		putc(*str);
		str++;
	}
}

void printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	printf_internal(putc, fmt, args);

	va_end(args);
}

