#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct
{
	uint8_t* buffer;
	size_t size; // Buffer size
} bitmap_t;

void BitmapSet(bitmap_t* bitmap, uint64_t index, bool enabled);
bool BitmapGet(bitmap_t* bitmap, uint64_t index);

