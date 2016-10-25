#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include <util/delay.h>
#include <string.h>

#include "usbdrv.h"
#include "usb.h"
#include "passwordSeed.h"

#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

#define digitalInput(x, y) { DDRD &= ~(1 << P ## x ## y); PORTD |= (1 << P ## x ## y); }
#define digitalRead(x, y) !(PIND & (1 << P ## x ## y))

typedef enum {
	FALSE,
	TRUE
} boolean_t;

typedef enum {
	STATE_DONE,
	STATE_SEND
} transmission_state_t;

static uchar messageState = STATE_DONE;
static void * messagePtr = NULL;
static uchar messageCharNext = TRUE;
static uchar messageRestoreCapsLock = FALSE;

static uchar buildReport() {
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
	digitalInput(D, 5);

	while (1){
		// keep the watchdog happy
		wdt_reset();
		usbPoll();
		if (digitalRead(D, 5) && messageState != STATE_SEND){
			messagePtr = NULL;
			messageState = STATE_SEND;
		}
		// characters are sent after the initial LED state from host to wait until device is recognized
		if (usbInterruptIsReady() && messageState == STATE_SEND && LedState != 0xff){
			messageState = buildReport();
			usbSetInterrupt((void *) &keyboardReport, sizeof (keyboardReport));
		}
	}
	return 0;
}
