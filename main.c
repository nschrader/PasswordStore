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

#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "usbdrv.h"
#include "usb.h"
#include "button.h"
#include "display.h"
#include "keyboard.h"

typedef enum {
	DISPLAY_MENU_PIN,
	DISPLAY_MENU_LANGUAGE,
	DISPLAY_MENU_SENT
} display_menu_t;

volatile uint8_t cycleCount = 0;
static volatile uint8_t innerCycleCount = 0;
static uint8_t displayDigitIndex = 0;
static uint8_t displayMenuIndex = DISPLAY_MENU_PIN;

static inline void hyperFast256CycleDevider() {
	if (!innerCycleCount)
		cycleCount++;
	innerCycleCount++;
}

static inline void keepWatchdogHappy() {
	wdt_reset();
	usbPoll();
}

static inline void processUSB() {
	// characters are sent after the initial LED state from host to wait until device is recognized
	if (usbInterruptIsReady() && messageState == STATE_SEND && LedState != 0xff){
		messageState = buildReport();
		usbSetInterrupt((void *) &keyboardReport, sizeof (keyboardReport));
	}
}

static inline void menuLogic() {
	if (displayMenuIndex == DISPLAY_MENU_PIN){
		if (buttonState == LONG_PRESS)
			displayMenuIndex = DISPLAY_MENU_LANGUAGE;
		else if (buttonState == SHORT_PRESS){
			displayDigitIndex++;
			if (displayDigitIndex == 4){
				displayMenuIndex = DISPLAY_MENU_SENT;
				startTransmission();
			}
		}
		countDisplay(displayDigitIndex);
	} else if (displayMenuIndex == DISPLAY_MENU_LANGUAGE){
		displayLanguage();
		if (buttonState == SHORT_PRESS){
			displayMenuIndex = DISPLAY_MENU_PIN;
			displayDigitIndex = 0;
			memset(displayRegisterIndex, _0, sizeof (displayRegisterIndex));
			countDisplay(displayDigitIndex);
		}
	} else // displayMenuIndex == DISPLAY_MENU_SENT
		displaySent();
}

static inline void setUpUSB() {
	memset(&keyboardReport, 0, sizeof (keyboardReport));
	wdt_enable(WDTO_1S); // enable 1s watchdog timer
	usbInit();
}

static inline void reconnectUSB() {
	// enforce re-enumeration after 500ms
	usbDeviceDisconnect();
	for (uint8_t i = 0; i < 250; i++){
		wdt_reset();
		_delay_ms(2);
	}
	usbDeviceConnect();
}

static void loopTasks() {
	hyperFast256CycleDevider();
	keepWatchdogHappy();
	processUSB();
	buttonPoll();
	menuLogic();
}

static inline void displayLoop() {
	while (TRUE){
		loopTasks();
		DIG4_OFF();
		writeDisplayRegister(displayRegister[displayRegisterIndex[0]]);
		DIG1_ON();

		loopTasks();
		DIG1_OFF();
		writeDisplayRegister(displayRegister[displayRegisterIndex[1]]);
		DIG2_ON();

		loopTasks();
		DIG2_OFF();
		writeDisplayRegister(displayRegister[displayRegisterIndex[2]]);
		DIG3_ON();

		loopTasks();
		DIG3_OFF();
		writeDisplayRegister(displayRegister[displayRegisterIndex[3]]);
		DIG4_ON();
	}
}

int main() {
	setUpUSB();
	reconnectUSB();

	// Enable interrupts after re-enumeration
	sei();

	// Set-up GPIOs
	digitalInputButton();

	// Set-up display
	DIG_OUTPUT();
	DIG_OFF();

	displayLoop();

	return 0;
}
