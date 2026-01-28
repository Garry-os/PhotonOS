#include "bitmap.h"

void BitmapSet(bitmap_t* bitmap, uint64_t index, bool enabled)
{
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0x80 >> bitIndex;
	
	// Clear the bit
	bitmap->buffer[byteIndex] &= ~(bitIndexer);

	if (enabled)
	{
		bitmap->buffer[byteIndex] |= bitIndexer;
	}
}

bool BitmapGet(bitmap_t* bitmap, uint64_t index)
{
	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0x80 >> bitIndex;

	if ((bitmap->buffer[byteIndex] & bitIndexer) > 0)
	{
		return true;
	}

	return false;
}

