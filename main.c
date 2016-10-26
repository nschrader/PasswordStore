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
#include "passwordSeed.h"

#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

typedef enum {
	STATE_DONE,
	STATE_SEND
} transmission_state_t;

#define DIG1_OFF() digitalWriteOn(D, 6)
#define DIG1_ON() digitalWriteOff(D, 6)
#define DIG2_OFF() digitalWriteOn(D, 0)
#define DIG2_ON() digitalWriteOff(D, 0)
#define DIG3_OFF() digitalWriteOn(D, 4)
#define DIG3_ON() digitalWriteOff(D, 4)
#define DIG4_OFF() digitalWriteOn(D, 1)
#define DIG4_ON() digitalWriteOff(D, 1)

#define DIG_OUTPUT() DDRD |= 0x53
#define DIG_OFF() PORTD |= 0x53

PROGMEM static const uchar _0 = 0xfa;
PROGMEM static const uchar _1 = 0x12;
PROGMEM static const uchar _2 = 0xce;
PROGMEM static const uchar _3 = 0x9e;
PROGMEM static const uchar _4 = 0x36;
PROGMEM static const uchar _5 = 0xbc;
PROGMEM static const uchar _6 = 0xfc;
PROGMEM static const uchar _7 = 0x1a;
PROGMEM static const uchar _8 = 0xfe;
PROGMEM static const uchar _9 = 0xbe;

#define _D _0
PROGMEM static const uchar _E = 0xec;
PROGMEM static const uchar _F = 0x6c;
PROGMEM static const uchar _N = 0x7a;
PROGMEM static const uchar _R = 0x7e;
PROGMEM static const uchar _RP = 0x7f;
#define _S _5

static uchar messageState = STATE_DONE;
static void * messagePtr = NULL;
static uchar messageCharNext = TRUE;
static uchar messageRestoreCapsLock = FALSE;

static uchar buildReport() {
	if (messageState == STATE_DONE || messagePtr >= NULL + PASSWORD_LENGTH){ // End of transmission
		if (messageRestoreCapsLock){
			keyboardReport.modifier =NO_MODIFIER;
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

volatile int32_t cycleCount = 0;

int main() {
	memset(&keyboardReport, 0, sizeof (keyboardReport));
	wdt_enable(WDTO_1S); // enable 1s watchdog timer
	usbInit();

	// enforce re-enumeration after 500ms
	usbDeviceDisconnect();
	for (uchar i = 0; i < 250; i++){
		wdt_reset();
		_delay_ms(2);
	}
	usbDeviceConnect();

	// Enable interrupts after re-enumeration
	sei();

	// Set-up GPIOs
	digitalInputButton();	
	digitalOutputRegister(B, 0xFF);
	
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
		if (cycleCount % 2) {
			DIG3_OFF();
			digitalWriteRegister(B, _N);
			DIG1_ON();
		} else {
			DIG1_OFF();
			digitalWriteRegister(B, _RP);
			DIG3_ON();
		}	
		cycleCount++;
	}
	return 0;
}
