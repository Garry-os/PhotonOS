#pragma once
#include <stdbool.h>

extern bool consoleInit; // Initialized or not?

void InitConsole();

void clearScreen();

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);

