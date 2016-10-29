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
#include "main.h"
#include <avr/io.h>
#include <stdint.h>

#define COUNT_1S_SHIFT 7

const __flash uint8_t displayRegister[] = {
	0x04, 0xfa, 0x12, 0xce, 0x9e, 0x36, 0xbc, 0xfc, 0x1a, 0xfe, 0xbe,
	0xec, 0x6c, 0x7a, 0x7e, 0x7f
};

static const __flash uint8_t languageMenu[] = {
	_D, _E, _E, _N, _F, _R
};

// Workaround for weird problem when using PORTB register and extern variables in macro
void writeDisplayRegister(uint8_t x) {
	PORTB = x;
}

static uint8_t countCycleCount = 0;
uint8_t displayRegisterIndex[] = {
	_0, _0, _0, _0
};

void countDisplay(uint8_t digit) {
	uint8_t diff = cycleCount - countCycleCount;
	if (diff >> COUNT_1S_SHIFT) { // Hyper fast (diff > 128)
		if (displayRegisterIndex[digit] == _9)
			displayRegisterIndex[digit] = _0;
		else
			displayRegisterIndex[digit]++;
		countCycleCount = cycleCount;
	}
}

void displaySent() {
	displayRegisterIndex[0] = _S;
	displayRegisterIndex[1] = _E;
	displayRegisterIndex[2] = _N;
	displayRegisterIndex[3] = _T;
}

static uint8_t menuPage = 0;
void displayLanguage() {
	uint8_t diff = cycleCount - countCycleCount;
	if (diff >> COUNT_1S_SHIFT) { // Hyper fast (diff > 128)
		displayRegisterIndex[0] = languageMenu[menuPage++];
		displayRegisterIndex[1] = languageMenu[menuPage++];
		displayRegisterIndex[2] = __;
		displayRegisterIndex[3] = __;
		countCycleCount = cycleCount;
	}
	if (menuPage == sizeof (languageMenu))
		menuPage = 0;
}
