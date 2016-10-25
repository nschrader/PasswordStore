#include "usbdrv.h"

typedef struct {
	uint8_t modifier;
	uint8_t reserved;
	uint8_t keycode[6];
} keyboard_report_t;

extern keyboard_report_t keyboardReport;
extern volatile uchar LedState;
