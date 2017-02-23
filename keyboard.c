/*
 * keyboard.c
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


#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>

#include "keyboard.h"
#include "display.h"
#include "main.h"
#include "usb.h"
#include "passwordSeed.h"
#include "random.h"
#include "conversionTable.h"
 
#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

typedef enum {
	LANG_EN = 2, LANG_DE = 4, LANG_FR = 6
} language_t;

static const __flash uint8_t *letterConversionTable;

static uint8_t correctLetter(passwordSeed *s) {
	if (menuPage == LANG_EN)
		return TRUE;
	for(uint8_t i = 0; i < letterConversionTableLenght; i++) {
		if (s->keycode == letterConversionTableEN[i]) {
			s->keycode = letterConversionTable[i];
			return TRUE;
		} else if (s->keycode < letterConversionTableEN[i])
			return FALSE;
	}
	return FALSE;
}

/* Conversion for digits:
 * EN	1-0
 * FR	Shift 1-0
 * DE	1-0
 */

static uint8_t correctDigit(passwordSeed *s) {
	if (menuPage == LANG_DE || menuPage == LANG_EN)
		return TRUE;
	if (s->modifier == 0 && s->keycode >= KEYCODE_1 && s->keycode <= KEYCODE_0) {
		s->modifier = 1;
		return TRUE;
	}
	return FALSE;
}

static const __flash uint8_t *signConversionTable;

static uint8_t correctSign(uint8_t *s) {
	if (menuPage == LANG_EN)
		return TRUE;
	for(uint8_t i = 0; i < signConversionTableLenght; i++) {
		if (*s == signConversionTableEN[i]) {
			*s = signConversionTable[i];
			return TRUE;
		} else if (*s < signConversionTableEN[i])
			return FALSE;
	}
	return FALSE;
} 

uint8_t messageState = STATE_DONE;
static void * messagePtr = NULL;
static uint8_t messageCharNext = TRUE;
static uint8_t messageRestoreCapsLock = FALSE;

uint8_t buildReport() {
	if (messageState == STATE_DONE || messagePtr >= NULL + PASSWORD_LENGTH*3) { // End of transmission
		if (messageRestoreCapsLock){
			keyboardReport.modifier = NO_MODIFIER;
			keyboardReport.keycode[0] = CAPS_LOCK_KEY;
			messageRestoreCapsLock = FALSE;
			LedState |= CAPS_LOCK_LED;
			return STATE_SEND;
		}
		memset(&keyboardReport, 0, sizeof (keyboardReport));
		return STATE_DONE;
	}
	if (LedState & CAPS_LOCK_LED){ // unlocks kaps
		keyboardReport.modifier = NO_MODIFIER;
		keyboardReport.keycode[0] = CAPS_LOCK_KEY;
		messageRestoreCapsLock = TRUE;
		LedState ^= CAPS_LOCK_LED;
		return STATE_SEND;
	}
	if (messageCharNext){ // send a keypress
		passwordSeed s;
		eeprom_read_block(&s, NULL + random2(), sizeof (passwordSeed));
		if (!correctSign((uint8_t*) &s)) {
			if (!correctLetter(&s)) {
				correctDigit(&s);
			}
		}
		keyboardReport.modifier = (s.modifier) ? SHIFT_MODIFIER : NO_MODIFIER;
		keyboardReport.keycode[0] = s.keycode;
		messagePtr++;
	} else // send a keyrelease
		memset(&keyboardReport, 0, sizeof (keyboardReport));
	messageCharNext = !messageCharNext; // invert
	return STATE_SEND;
}

void startTransmission() {
	messagePtr = NULL + PASSWORD_LENGTH*2;
	messageState = STATE_SEND;
	srandom2(getPIN());
	if (menuPage == LANG_EN) {
		letterConversionTable = letterConversionTableEN;
		signConversionTable = signConversionTableEN;
	}  else if (menuPage == LANG_DE) { 
		letterConversionTable = letterConversionTableDE;
		signConversionTable = signConversionTableDE;
	} else { // language == LANG_FR
		letterConversionTable = letterConversionTableFR;
		signConversionTable = signConversionTableFR;
	}	
}
