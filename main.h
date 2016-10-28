/*
 * main.h
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

#ifndef _MAIN_H
#define _MAIN_H

#include <avr/io.h>
#include "usbdrv.h"

#define digitalInput(x, y) DDR ## x &= ~(1 << P ## x ## y)
#define digitalPullup(x, y) PORT ## x |= (1 << P ## x ## y)
#define digitalOutput(x, y) DDR ## x |= (1 << P ## x ##y)
#define digitalRead(x, y) !(PIN ## x & (1 << P ## x ## y))
#define digitalWriteOn(x, y) PORT ## x |= (1 << P ## x ## y)
#define digitalWriteOff(x, y) PORT ## x &= ~(1 << P ## x ## y)

typedef enum {
	FALSE,
	TRUE
} boolean_t;

extern volatile uchar cycleCount;

#endif
