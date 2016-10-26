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
#include "usbdrv.h"

#define BUTTON_LONG_PRESS_THRESHOLD 50000
#define BUTTON_DEBOUNCING_THRESHOLD 100

uchar buttonState = NO_PRESS;
static uchar lastButtonState = NO_PRESS;
static int32_t buttonPushCycleCount = 0;
static uchar lastButton = FALSE;

void buttonPoll() {
	uchar sameButtonState = FALSE;
	uchar button = digitalReadButton();
	if (button){
		if (!lastButton)
			buttonPushCycleCount = cycleCount;
		int32_t diff = cycleCount - buttonPushCycleCount;
		if (diff > BUTTON_LONG_PRESS_THRESHOLD)
			buttonState = LONG_PRESS;
		else if (diff > BUTTON_DEBOUNCING_THRESHOLD)
			buttonState = SHORT_PRESS;
		sameButtonState = (lastButtonState == buttonState);
	}
	lastButtonState = buttonState;
	if ((!button && lastButton) || sameButtonState)
		buttonState = NO_PRESS;
	lastButton = button;
}
