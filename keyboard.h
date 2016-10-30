#include "passwordSeed.h"
#include <stdint.h>

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

typedef enum {
	STATE_DONE,
	STATE_SEND
} transmission_state_t;

extern uint8_t messageState;

uint8_t buildReport();
void startTransmission();

#endif
