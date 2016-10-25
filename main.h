#ifndef _MAIN_H
#define _MAIN_H

#include <avr/io.h>

#define digitalInput(x, y) { DDR ## x &= ~(1 << P ## x ## y); PORT ## x |= (1 << P ## x ## y); }
#define digitalRead(x, y) !(PIN ## x & (1 << P ## x ## y))

typedef enum {
	FALSE,
	TRUE
} boolean_t;

extern volatile int32_t cycleCount;

#endif
