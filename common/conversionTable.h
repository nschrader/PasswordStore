/*
 * conversionTable.h
 * 
 * Copyright 2017 Nick Schrader <nick.schrader@mailbox.org>
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
 
#ifndef _CONVERSION_TABLE_H
#define _CONVERSION_TABLE_H

#include "passwordSeed.h"

#define LOWER_LETTER_ASCII 0x61
#define UPPER_LETTER_ASCII 0x41
#define LOWER_DIGIT_ASCII 0x30

#define USB_HID_CHAR_OVERHEAD 0x04
#define USB_HID_DIGIT_OVERHEAD 0x1d

#define KEYCODE_1 0x1e
#define KEYCODE_0 0x27

#define CS(x) ((x << 2) + 0x02) // USB keycode to upper case passwordSeed
#define LS(x) (x << 2) // USB keycode to lower case passwordSeed

#ifndef __AVR__
#define __flash
#endif

extern const __flash uint8_t signConversionTableEN[];
extern const __flash uint8_t signConversionTableFR[];
extern const __flash uint8_t signConversionTableDE[];
extern const char signConversionTableASCII[];

extern const __flash uint8_t letterConversionTableEN[];
extern const __flash uint8_t letterConversionTableFR[];
extern const __flash uint8_t letterConversionTableDE[];

extern const __flash uint8_t signConversionTableLenght;
extern const __flash uint8_t letterConversionTableLenght;

#endif
