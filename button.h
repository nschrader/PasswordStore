#ifndef _BUTTON_H
#define _BUTTON_H

#include "main.h"
#include "usbdrv.h"

#define BUTTON_LONG_PRESS_THRESHOLD 50000
#define BUTTON_DEBOUNCING_THRESHOLD 100

#define digitalInputButton() digitalInput(D, 5)
#define digitalReadButton() digitalRead(D, 5)

typedef enum {
	NO_PRESS,
	SHORT_PRESS,
	LONG_PRESS
} button_state_t;

extern uchar buttonState;

void buttonPoll();

#endif
