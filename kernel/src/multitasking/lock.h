#pragma once
#include <stdbool.h>

typedef bool lock_t;

void lockAcquire(lock_t* lock);
void lockRelease(lock_t* lock);

