#include "ctype.h"

// Is lower case or not?
bool islower(char c)
{
	return c >= 'a' && c <= 'z';
}

// Convert to uppercase
char toupper(char c)
{
	return islower(c) ? (c - 'a' + 'A') : c;
}

