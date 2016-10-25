#include "button.h"
#include "main.h"
#include "usbdrv.h"

uchar buttonState = NO_PRESS;
static uchar lastButtonState = NO_PRESS;
static int32_t buttonPushCycleCount = 0;
static uchar lastButton = FALSE;

void buttonPoll() {
	uchar sameButtonState = FALSE;
	uchar button = digitalReadButton();
	if (button) {
		if (!lastButton)
			buttonPushCycleCount = cycleCount;
		int32_t diff = cycleCount - buttonPushCycleCount;
		if (diff > BUTTON_LONG_PRESS_THRESHOLD)
			buttonState = LONG_PRESS;
		else if (diff > BUTTON_DEBOUNCING_THRESHOLD)
			buttonState = SHORT_PRESS;
		sameButtonState = (lastButtonState == buttonState);
	}
	lastButtonState = buttonState;
	if ((!button && lastButton) || sameButtonState)
		buttonState = NO_PRESS;
	lastButton = button;
}
