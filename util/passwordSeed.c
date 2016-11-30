/*
 * passwordSeed.c
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
 * This program generates a password seed to be flashed into an EEPROM
 * with PASSWORD_POOL bytes. Each seedbyte is generated in the form:
 * {USB-HID-Keycode} {0/1 (SMALL/CAPITAL)} {0}, i.e. 01100110
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "passwordSeed.h"

#define isLetter(x) (x >= 0x04 && x <= 0x1d)
#define isDigitUpperCase(x) ((x >= 0x24 && x <= 0x27) || x == 0x21 || x == 0x22)
#define isDigitLowerCase(x) ((x >= 0x1e && x <= 0x20) || x == 0x23)
#define isSignUpperCase(x) (x == 0x2d || x == 0x2e || x == 0x33 || x == 0x34 || x == 0x38)
#define isSignLowerCase(x) (x == 0x36 || x == 0x37)
#define validKeycode(x) (isLetter(x) || isDigitUpperCase(x) || \
	                     isDigitLowerCase(x) || isSignUpperCase(x) || isSignLowerCase(x))

static inline FILE *openFile(int argc, char *argv[]) {
	FILE *out;
	if (argc > 1)
		out = fopen(argv[1], "w+b");
	else
		out = fopen("eeprom.bin", "w+b");
	if (out == NULL)
		exit(EXIT_FAILURE);
	return out;
}

static inline void generateSeed(passwordSeed s[]) {
	memset(s, 0, sizeof (s));
	for (int c_modifier = 0, c_keycode = 0; c_modifier < PASSWORD_POOL || c_keycode < PASSWORD_POOL; ){
		uchar ch = getchar();
		if (validKeycode(ch) && c_keycode < PASSWORD_POOL)
			s[c_keycode++].keycode = ch;
		else if (c_modifier < PASSWORD_POOL)
			s[c_modifier++].modifier =
			    (isDigitLowerCase(s[c_keycode].keycode) && isSignLowerCase(s[c_keycode].keycode)) ? 0 : (ch % 2);
		c_modifier++;
	}
}

int main(int argc, char * argv[]) {
	passwordSeed s[PASSWORD_POOL];
	FILE * out = openFile(argc, argv);
	generateSeed(s);
	fwrite(s, sizeof (passwordSeed), PASSWORD_POOL, out);
	return EXIT_SUCCESS;
}
