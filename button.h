/*
 * button.h
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

#ifndef _BUTTON_H
#define _BUTTON_H

#include "main.h"
#include "usbdrv.h"

#define digitalInputButton() { digitalInput(D, 5); digitalPullup(D, 5); }
#define digitalReadButton() digitalRead(D, 5)

typedef enum {
	NO_PRESS,
	SHORT_PRESS,
	LONG_PRESS
} button_state_t;

extern uchar buttonState;

void buttonPoll();

#endif
