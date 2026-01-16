//
// Kernel starting entry file
//
#include <x86_64/system.h>

void main()
{
	x86_outb(0xE9, 'e');
	while (1);
}
