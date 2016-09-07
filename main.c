#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include <util/delay.h>
#include <string.h>

#include "usbdrv.h"
#include "passwordSeed.h"

#define TRUE 1
#define FALSE 0

#define STATE_SEND 1
#define STATE_DONE 0

#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x75, 0x01, // REPORT_SIZE (1)
	0x95, 0x08, // REPORT_COUNT (8)
	0x05, 0x07, // USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)(224)
	0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)(231)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x01, // LOGICAL_MAXIMUM (1)
	0x81, 0x02, // INPUT (Data,Var,Abs) ; Modifier byte
	0x95, 0x01, // REPORT_COUNT (1)
	0x75, 0x08, // REPORT_SIZE (8)
	0x81, 0x03, // INPUT (Cnst,Var,Abs) ; Reserved byte
	0x95, 0x05, // REPORT_COUNT (5)
	0x75, 0x01, // REPORT_SIZE (1)
	0x05, 0x08, // USAGE_PAGE (LEDs)
	0x19, 0x01, // USAGE_MINIMUM (Num Lock)
	0x29, 0x05, // USAGE_MAXIMUM (Kana)
	0x91, 0x02, // OUTPUT (Data,Var,Abs) ; LED report
	0x95, 0x01, // REPORT_COUNT (1)
	0x75, 0x03, // REPORT_SIZE (3)
	0x91, 0x03, // OUTPUT (Cnst,Var,Abs) ; LED report padding
	0x95, 0x06, // REPORT_COUNT (6)
	0x75, 0x08, // REPORT_SIZE (8)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x65, // LOGICAL_MAXIMUM (101)
	0x05, 0x07, // USAGE_PAGE (Keyboard)(Key Codes)
	0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))(0)
	0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)(101)
	0x81, 0x00, // INPUT (Data,Ary,Abs)
	0xc0 // END_COLLECTION
};

typedef struct {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
} keyboard_report_t;

static keyboard_report_t keyboard_report; // sent to host
volatile static uchar LED_state = 0xff; // received from host
static uchar idleRate; // repeat rate for keyboards

static uchar messageState = STATE_DONE;
static void *messagePtr = NULL;
static uchar messageCharNext = TRUE;
static uchar messageRestoreCapsLock = FALSE;

uchar buildReport() {
	if (messageState == STATE_DONE || messagePtr >= NULL + PASSWORD_LENGTH) { // End of transmission
		if (messageRestoreCapsLock) {
			keyboard_report.modifier = NO_MODIFIER;
			keyboard_report.keycode[0] = CAPS_LOCK_KEY;
		} else
			memset(&keyboard_report, 0, sizeof (keyboard_report));
		return STATE_DONE;
	}
	if (LED_state & CAPS_LOCK_LED) { // unlocks kaps
		messageRestoreCapsLock = TRUE;
		keyboard_report.modifier = NO_MODIFIER;
		keyboard_report.keycode[0] = CAPS_LOCK_KEY;
		return STATE_SEND;
	}
	if (messageCharNext){ // send a keypress
		passwordSeed s;
		eeprom_read_block(&s, messagePtr++, sizeof(passwordSeed));
		keyboard_report.modifier = (s.modifier) ? SHIFT_MODIFIER : NO_MODIFIER;
		keyboard_report.keycode[0] = s.keycode;
	} else // send a keyrelease
		memset(&keyboard_report, 0, sizeof (keyboard_report));
	messageCharNext = !messageCharNext; // invert
	return STATE_SEND;
}

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t * rq = (void *) data;
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
		switch (rq->bRequest){
		case USBRQ_HID_GET_REPORT: // send "no keys pressed" if asked here
			usbMsgPtr = (void *) &keyboard_report; // we only have this one
			memset(&keyboard_report, 0, sizeof (keyboard_report));
			return sizeof (keyboard_report);
		case USBRQ_HID_SET_REPORT: // if wLength == 1, should be LED state
			return (rq->wLength.word == 1) ? USB_NO_MSG : 0;
		case USBRQ_HID_GET_IDLE: // send idle rate to PC as required by spec
			usbMsgPtr = &idleRate;
			return 1;
		case USBRQ_HID_SET_IDLE: // save idle rate as required by spec
			idleRate = rq->wValue.bytes[1];
			return 0;
		}
	}
	return 0;
}

void caps_toggle() {
	messagePtr = 0;
	messageState = STATE_SEND;
}

usbMsgLen_t usbFunctionWrite(uint8_t * data, uchar len) {
	if (data[0] == LED_state)
		return 1;
	else
		LED_state = data[0];
	caps_toggle();
	return 1; // Data read, not expecting more
}

int main() {
	memset(&keyboard_report, 0, sizeof (keyboard_report));
	wdt_enable(WDTO_1S); // enable 1s watchdog timer
	usbInit();

	// enforce re-enumeration after 500ms
	usbDeviceDisconnect();
	for (uchar i = 0; i < 250; i++){
		wdt_reset();
		_delay_ms(2);
	}
	usbDeviceConnect();

	sei(); // Enable interrupts after re-enumeration

	while (1){
		// keep the watchdog happy
		wdt_reset();
		usbPoll();

		// characters are sent after the initial LED state from host to wait until device is recognized
		if (usbInterruptIsReady() && messageState == STATE_SEND && LED_state != 0xff){
			messageState = buildReport();
			usbSetInterrupt((void *) &keyboard_report, sizeof (keyboard_report));
		}
	}

	return 0;
}
