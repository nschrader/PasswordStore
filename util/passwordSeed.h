/*
 * passwordSeed.h
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


#ifndef _PASSWORD_SEED_H
#define _PASSWORD_SEED_H

#define PASSWORD_LENGTH 16
#define PASSWORD_POOL 256

#define isLetter(x) (x >= 0x04 && x <= 0x1d)
#define isDigitUpperCase(x) ((x >= 0x24 && x <= 0x27) || x == 0x21 || x == 0x22)
//TODO: Looks wierd too... Why didn't I permit 4, 5 7, 8, 9 and 0?
#define isDigitLowerCase(x) (x >= 0x1e && x <= 0x27)
#define isSignUpperCase(x) (x == 0x2d || x == 0x2e || x == 0x33 || x == 0x34 || x == 0x38)
#define isSignLowerCase(x) (x == 0x36 || x == 0x37)

#define validKeycode(x) (isLetter(x) || isDigitUpperCase(x) || \
	                     isDigitLowerCase(x) || isSignUpperCase(x) || isSignLowerCase(x))
#define validOnlyLower(x) ((isDigitLowerCase(x) && !isDigitUpperCase(x)) || \
							(isSignLowerCase(x) && !isSignUpperCase(x)))
#define validOnlyUpper(x) ((!isDigitLowerCase(x) && isDigitUpperCase(x)) || \
							(!isSignLowerCase(x) && isSignUpperCase(x)))

#ifndef uchar
typedef unsigned char uchar;
#endif

#pragma pack(1)
typedef struct {
  uchar :1;
  uchar modifier:1;
  uchar keycode:6;
} passwordSeed;

#endif
