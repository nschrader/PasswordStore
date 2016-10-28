/*
 * button.c
 *
 * Copyright 2016 Nick Schrader <nick.schrader@mailbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "button.h"
#include "main.h"

#define BUTTON_LONG_PRESS_SHIFT 7
#define BUTTON_DEBOUNCING_SHIFT 4

uint8_t buttonState = NO_PRESS;
static uint8_t lastButtonState = NO_PRESS;
static uint8_t buttonPushCycleCount = 0;
static uint8_t lastButton = FALSE;

void buttonPoll() {
	uint8_t sameButtonState = FALSE;
	uint8_t button = digitalReadButton();
	if (button){
		if (!lastButton)
			buttonPushCycleCount = cycleCount;
		uint8_t diff = cycleCount - buttonPushCycleCount;
		if (diff >> BUTTON_LONG_PRESS_SHIFT) //Hyper fast diff == 256
			buttonState = LONG_PRESS;
		else if (diff >> BUTTON_DEBOUNCING_SHIFT) // Hyper fast diff == 16
			buttonState = SHORT_PRESS;
		sameButtonState = (lastButtonState == buttonState);
	}
	lastButtonState = buttonState;
	if ((!button && lastButton) || sameButtonState)
		buttonState = NO_PRESS;
	lastButton = button;
}
