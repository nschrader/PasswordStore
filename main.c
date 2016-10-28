/*
 * main.c
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

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "usbdrv.h"
#include "usb.h"
#include "button.h"
#include "display.h"
#include "passwordSeed.h"

#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

typedef enum {
	STATE_DONE,
	STATE_SEND
} transmission_state_t;

static uint8_t messageState = STATE_DONE;
static void * messagePtr = NULL;
static uint8_t messageCharNext = TRUE;
static uint8_t messageRestoreCapsLock = FALSE;

static uint8_t buildReport() {
	if (messageState == STATE_DONE || messagePtr >= NULL + PASSWORD_LENGTH){ // End of transmission
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
		eeprom_read_block(&s, messagePtr++, sizeof (passwordSeed));
		keyboardReport.modifier = (s.modifier) ? SHIFT_MODIFIER : NO_MODIFIER;
		keyboardReport.keycode[0] = s.keycode;
	} else // send a keyrelease
		memset(&keyboardReport, 0, sizeof (keyboardReport));
	messageCharNext = !messageCharNext; // invert
	return STATE_SEND;
}

volatile uint8_t cycleCount = 0;
static uint8_t innerCycleCount = 0;

int main() {
	memset(&keyboardReport, 0, sizeof (keyboardReport));
	wdt_enable(WDTO_1S); // enable 1s watchdog timer
	usbInit();

	// enforce re-enumeration after 500ms
	usbDeviceDisconnect();
	for (uint8_t i = 0; i < 250; i++){
		wdt_reset();
		_delay_ms(2);
	}
	usbDeviceConnect();

	// Enable interrupts after re-enumeration
	sei();

	// Set-up GPIOs
	digitalInputButton();

	// Set-up display
	DIG_OUTPUT();
	DIG_OFF();

	while (1){
		// keep the watchdog happy
		wdt_reset();
		usbPoll();
		buttonPoll();
		if (buttonState == LONG_PRESS && messageState != STATE_SEND){
			messagePtr = NULL;
			messageState = STATE_SEND;
		}
		// characters are sent after the initial LED state from host to wait until device is recognized
		if (usbInterruptIsReady() && messageState == STATE_SEND && LedState != 0xff){
			messageState = buildReport();
			usbSetInterrupt((void *) &keyboardReport, sizeof (keyboardReport));
		}
		if (cycleCount % 2){
			DIG3_OFF();
			writeDisplayRegister(displayRegister[0]);
			DIG1_ON();
		} else {
			DIG1_OFF();
			writeDisplayRegister(displayRegister[5]);
			DIG3_ON();
		}
		// Hyper fast 256-devider
		if (!innerCycleCount)
			cycleCount++;
		innerCycleCount++;		
	}
	return 0;
}
