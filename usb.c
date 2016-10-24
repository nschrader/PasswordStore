#include <string.h>
#include "usb.h"
#include "usbdrv.h"

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

keyboard_report_t keyboardReport; // sent to host
volatile uchar LedState = 0xff; // received from host
uchar idleRate; // repeat rate for keyboards

usbMsgLen_t usbFunctionSetup(uchar data[8]) {
	usbRequest_t * rq = (void *) data;
	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){
		switch (rq->bRequest){
		case USBRQ_HID_GET_REPORT: // send "no keys pressed" if asked here
			usbMsgPtr = (void *) &keyboardReport; // we only have this one
			memset(&keyboardReport, 0, sizeof (keyboardReport));
			return sizeof (keyboardReport);
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

usbMsgLen_t usbFunctionWrite(uint8_t * data, uchar len) {
	LedState = data[0];
	return 1;
}
