/*
 * passwordDecrypt.c
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "passwordSeed.h"
#include "random.h"
#include "conversionTable.h"

#define CONVERSION_TABLE_ERROR 0

static inline passwordSeed *fileOpen(int argc, char *argv[]) {
	passwordSeed *s = malloc(sizeof(passwordSeed) * PASSWORD_POOL);
	if (argc < 2)
		exit(EXIT_FAILURE);
	FILE *in = fopen(argv[1], "r");
	if (in == NULL)
		exit(EXIT_FAILURE);
	if (fread(s, sizeof(passwordSeed), PASSWORD_POOL, in) != PASSWORD_POOL)
		exit(EXIT_FAILURE);
	return s;
}

static inline int getPIN() {
	char pin[4];
	scanf("%4c", pin);
	if (!memcmp(pin, "0000", 4))
		return 0;
	int r = strtol(pin, NULL, 10);
	if (!r)
		exit(EXIT_FAILURE);
	return r;
}

static inline char useSignConversionTable(passwordSeed s) {
	for(int i = 0; i < signConversionTableLenght; i++) {
		if (memcmp(&s, &signConversionTableEN[i], sizeof(passwordSeed)) == 0)
			return signConversionTableASCII[i];
		else if (memcmp(&s, &signConversionTableEN[i], sizeof(passwordSeed)) < 0)
			return CONVERSION_TABLE_ERROR;
	}
	return CONVERSION_TABLE_ERROR;
}

static inline char handleDigits(passwordSeed s) {
	if (s.keycode == 0x27 && s.modifier == 0)
		return '0';
	else
		return LOWER_DIGIT_ASCII + s.keycode - USB_HID_DIGIT_OVERHEAD;
}

static inline char convertKeycode(passwordSeed s) {
	if (isLetter(s.keycode))
		return ((s.modifier) ? UPPER_LETTER_ASCII : LOWER_LETTER_ASCII) + s.keycode - USB_HID_CHAR_OVERHEAD;
	if (s.modifier == 0 && isDigitLowerCase(s.keycode)) {
		return handleDigits(s);
	}
	return useSignConversionTable(s);
}

static inline void printPIN(passwordSeed *s) {
	char password[PASSWORD_LENGTH + 1];
	for(int i = 0; i < PASSWORD_LENGTH; i++) {
		password[i] = convertKeycode(s[random2()]);
		if(password[i] == CONVERSION_TABLE_ERROR) {
			printf("Conversion table error!\n");
			exit(EXIT_FAILURE);
		}
	}
	password[PASSWORD_LENGTH] = '\0';
	printf("%s\n", password);
}

int main(int argc, char *argv[]) {
	passwordSeed *s = fileOpen(argc, argv);
	srandom2(getPIN());
	printPIN(s);
	return EXIT_SUCCESS;
}
