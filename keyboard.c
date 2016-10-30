#include <avr/eeprom.h>
#include <stdint.h>
#include <string.h>

#include "keyboard.h"
#include "display.h"
#include "main.h"
#include "usb.h"
#include "passwordSeed.h"

/* Conversion table for letters:
 * EN	A	M	Q	W	Y	Z
 * FR	Q	;	A	Z	Y	W	
 * DE	A	M	Q	W	Z	Y
 * 
 * Conversion for digits:
 * EN	1-0
 * FR	Shift 1-0
 * DE	1-0
 * 
 * Conversion table for signs:
 * Sign			$	%	&	*	(	)	-	_	=	+	;	:	'	"	.	,	/	?
 * EN									-		=		;		'		.	,	/
 * EN Shift		4	5	7	8	9	0		-		=		;		'				/
 * FR			]		1	\	5	-	6	8	=		,	.	4	3		M
 * FR Shift			'								=					,		.	M
 * DE									/						\		.	,	
 * DE Shift		4	5	6	[	8	9		/	0	]	,	.		2			7	-
 */
 
#define CAPS_LOCK_LED 0x02
#define CAPS_LOCK_KEY 0x39

#define SHIFT_MODIFIER 0x20
#define NO_MODIFIER 0x00

typedef enum {
	LANG_EN = 1, LANG_DE, LANG_FR
} language_t;
static uint8_t language = LANG_EN;
 
static const __flash uint8_t letterConversationTableEN[] = {
	0x04, 0x10, 0x14, 0x1a, 0x1c, 0x1d
};

static const __flash uint8_t letterConversationTableFR[] = {
	0x14, 0x33, 0x04, 0x1d, 0x1c, 0x1a
};

static const __flash uint8_t letterConversationTableDE[] = {
	0x04, 0x10, 0x14, 0x1a, 0x1d, 0x1c
};

static const __flash uint8_t *letterConversationTable;

static uint8_t correctLetter(passwordSeed *s) {
	if (language == LANG_EN)
		return TRUE;
	for(uint8_t i = 0; i < sizeof(letterConversationTableEN); i++) {
		if (s->keycode == letterConversationTableEN[i]) {
			s->keycode = letterConversationTable[i];
			return TRUE;
		} else if (s->keycode < letterConversationTableEN[i])
			return FALSE;
	}
	return FALSE;
}

static uint8_t correctDigit(passwordSeed *s) {
	if (language != LANG_FR)
		return TRUE;
	if (!s->modifier && s->keycode >= 0x1e && s->keycode <= 0x27) {
		s->modifier = 1;
		return TRUE;
	}
	return FALSE;
}

#define CS(x) ((x << 2) + 0x02) // USB keycode to capital case passwordSeed
#define LS(x) (x << 2) // USB keycode to lower case passwordSeed

static const __flash uint8_t signConversionTableEN[] = {
	CS(0x21), CS(0x22), CS(0x24), CS(0x25), CS(0x26), CS(0x27), 
	LS(0x2d), CS(0x2d), LS(0x2e), CS(0x2e), LS(0x33), CS(0x33),
	LS(0x34), CS(0x34), LS(0x36), LS(0x37), LS(0x38), CS(0x38)
};

static const __flash uint8_t signConversionTableFR[] = {
	LS(0x30), CS(0x34), LS(0x1e), LS(0x31), LS(0x22), LS(0x2d), 
	LS(0x23), LS(0x25), LS(0x2e), CS(0x2e), LS(0x36), LS(0x37),
	LS(0x21), LS(0x20), CS(0x36), LS(0x10), CS(0x37), CS(0x10)
};

static const __flash uint8_t signConversionTableDE[] = {
	CS(0x21), CS(0x22), CS(0x23), CS(0x2f), CS(0x25), CS(0x26), 
	LS(0x38), CS(0x38), CS(0x27), CS(0x30), CS(0x36), CS(0x37),
	LS(0x31), CS(0x1f), LS(0x37), LS(0x36), CS(0x24), CS(0x2d)
};

static const __flash uint8_t *signConversionTable;

static uint8_t correctSign(uint8_t *s) {
	if (language == LANG_EN)
		return TRUE;
	for(uint8_t i = 0; i < sizeof (signConversionTableEN); i++) {
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
	if (messageState == STATE_DONE || messagePtr >= NULL + PASSWORD_LENGTH) { // End of transmission
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
		/*if (!correctLetter(&s)) {
			if (!correctDigit(&s))
				;//correctSign((uint8_t*) &s);
		}*/
		correctLetter(&s);
		correctDigit(&s);
		correctSign((uint8_t*) &s);
		keyboardReport.modifier = (s.modifier) ? SHIFT_MODIFIER : NO_MODIFIER;
		keyboardReport.keycode[0] = s.keycode;
	} else // send a keyrelease
		memset(&keyboardReport, 0, sizeof (keyboardReport));
	messageCharNext = !messageCharNext; // invert
	return STATE_SEND;
}

void startTransmission() {
	messagePtr = NULL;
	messageState = STATE_SEND;
	language = menuPage / 2;
	if (language == LANG_EN) {
		letterConversationTable = letterConversationTableEN;
		signConversionTable = signConversionTableEN;
	}  else if (language == LANG_DE) { 
		letterConversationTable = letterConversationTableDE;
		signConversionTable = signConversionTableDE;
	} else { // language == LANG_FR
		letterConversationTable = letterConversationTableFR;
		signConversionTable = signConversionTableFR;
	}	
}
