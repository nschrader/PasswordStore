#include <stdint.h>

#ifndef _DISPLAY_H
#define _DISPLAY_H

#define DIG1_OFF() digitalWriteOn(D, 6)
#define DIG1_ON() digitalWriteOff(D, 6)
#define DIG2_OFF() digitalWriteOn(D, 0)
#define DIG2_ON() digitalWriteOff(D, 0)
#define DIG3_OFF() digitalWriteOn(D, 4)
#define DIG3_ON() digitalWriteOff(D, 4)
#define DIG4_OFF() digitalWriteOn(D, 1)
#define DIG4_ON() digitalWriteOff(D, 1)

#define DIG_OUTPUT() { DDRD |= 0x53; DDRB = 0xff; }
#define DIG_OFF() PORTD |= 0x53

extern __flash const uint8_t displayRegister[];
void writeDisplayRegister(uint8_t x);

#define _D _0
#define _S _5
typedef enum {
	_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _E, _F, _N, _R, _RP
} display_character_t;

#endif
