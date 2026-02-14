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

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define Backspace 0x0E
#define Spacebar 0x39
#define Tab 0x0F

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

const char UppercaseTable[] = {
	0 ,  0 , '!', '@',
   '#', '$', '%', '^',
   '&', '*', '(', ')',
   '_', '+',  0 ,  0 ,
   'Q', 'W', 'E', 'R',
   'T', 'Y', 'U', 'I',
   'O', 'P', '{', '}',
	0 ,  0 , 'A', 'S',
   'D', 'F', 'G', 'H',
   'J', 'K', 'L', ':',
   '"',  0 , '|',
   'Z', 'X', 'C', 'V',
   'B', 'N', 'M', '<',
   '>', '?',  0 , '*',
	0 , ' '
};

char ScancodeToASCII(uint8_t scancode, bool uppercase)
{
	if (scancode > 58) return 0;

	if (uppercase)
	{
		return UppercaseTable[scancode];
	}

	return ASCIITable[scancode];
}

void keyboardHandler(cpu_registers_t* context)
{
	(void)context;

	uint8_t scancode = x86_inb(PS2_PORT);

	char key = ScancodeToASCII(scancode, g_KeyInfo->uppercase);

	switch (scancode)
	{
		case Enter:
			key = '\n';
			break;
		case LeftShift:
			g_KeyInfo->uppercase = true;
			return;
		case RightShift:
			g_KeyInfo->uppercase = true;
			return;
		case LeftShift + 0x80:
			// Release
			g_KeyInfo->uppercase = false;
			return;
		case RightShift + 0x80:
			// Release
			g_KeyInfo->uppercase = false;
			return;
		case Backspace:
			key = '\b';
			break;
		case Tab:
			key = '\t';
			break;
	}

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

