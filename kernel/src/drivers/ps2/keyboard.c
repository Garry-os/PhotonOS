//
// Basic PS/2 keyboard driver implementation
//
#include "keyboard.h"
#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include <utils/memory.h>
#include <x86_64/cpu.h>
#include <x86_64/irq.h>

#define PS2_PORT 0x60

typedef struct
{
	char key;
	bool uppercase;
} keyInfo_t;

keyInfo_t* g_KeyInfo = NULL;

const char ASCIITable[] = {
	0 ,  0 , '1', '2',
   '3', '4', '5', '6',
   '7', '8', '9', '0',
   '-', '=',  0 ,  0 ,
   'q', 'w', 'e', 'r',
   't', 'y', 'u', 'i',
   'o', 'p', '[', ']',
	0 ,  0 , 'a', 's',
   'd', 'f', 'g', 'h',
   'j', 'k', 'l', ';',
   '\'','`',  0 , '\\',
   'z', 'x', 'c', 'v',
   'b', 'n', 'm', ',',
   '.', '/',  0 , '*',
	0 , ' '
};

char ScancodeToASCII(uint8_t scancode, bool uppercase)
{
	if (scancode > 58) return 0;

	char c = ASCIITable[scancode];

	if (uppercase)
	{
		return c - 0x20;
	}

	return c;
}

void keyboardHandler(cpu_registers_t* context)
{
	(void)context;

	uint8_t scancode = x86_inb(PS2_PORT);

	char key = ScancodeToASCII(scancode, false);

	if (key != 0)
	{
		g_KeyInfo->key = key;
	}
}

void InitPS2Keyboard()
{
	g_KeyInfo = malloc(sizeof(keyInfo_t));
	memset(g_KeyInfo, 0, sizeof(keyInfo_t));

	IRQ_RegisterHandler(1, keyboardHandler);
}

// This function polls until the user pressed a key
// Returns the key
char getKey()
{
	while (!g_KeyInfo->key)
	{
		asm volatile ("hlt");
	}

	char c = g_KeyInfo->key;
	g_KeyInfo->key = 0;

	return c;
}

