/*
 * display.h
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

#include <stdint.h>

#ifndef _DISPLAY_H
#define _DISPLAY_H

#define DIG1_OFF() digitalWriteOn(D, 6)
#define DIG1_ON() digitalWriteOff(D, 6)
#define DIG2_OFF() digitalWriteOn(D, 0)
#define DIG2_ON() digitalWriteOff(D, 0)
#define DIG3_OFF() digitalWriteOn(D, 4)
#define DIG3_ON() digitalWriteOff(D, 4)
#define DIG4_OFF() digitalWriteOn(D, 1)
#define DIG4_ON() digitalWriteOff(D, 1)

#define DIG_OUTPUT() { DDRD |= 0x53; DDRB = 0xff; }
#define DIG_OFF() PORTD |= 0x53

extern __flash const uint8_t displayRegister[];
void writeDisplayRegister(uint8_t x);

#define _D _0
#define _S _5
typedef enum {
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _E, _F, _N, _R, _RP
} display_character_t;

#endif
