/*
 * display.c
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

#include "display.h"
#include <avr/io.h>
#include <stdint.h>

const __flash uint8_t displayRegister[] = {
	0xfa, 0x12, 0xce, 0x9e, 0x36, 0xbc, 0xfc, 0x1a, 0xfe, 0xbe, 0xec,
	0x6c, 0x7a, 0x7e, 0x7f
};

void writeDisplayRegister(uint8_t x) {
	PORTB = x;
}
