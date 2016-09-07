/*
 * passwordSeed.c
 * 
 * This program generates a password seed to be flashed into an EEPROM
 * with PASSWORD_POOL bytes. Each seedbyte is generated in the form:
 * {USB-HID-Keycode} {0/1 (SMALL/CAPITAL)} {0}, i.e. 01100110
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "passwordSeed.h"

#define validKeycode(x) ((x >= 0x04 && x <= 0x27) || (x >= 0x2d && x <= 0x31) || (x >= 0x33 && x <= 0x37))

int main(int argc, char *argv[]) {
  passwordSeed s[PASSWORD_POOL];
  FILE *out;
  
  if (argc > 1)
    out = fopen(argv[1], "w+b");
  else
    out = fopen("eeprom.bin", "w+b");
  if (out == NULL)
    return EXIT_FAILURE;
  
  memset(s, 0, sizeof(s));
  for (int c_modifier = 0, c_keycode = 0; c_modifier < PASSWORD_POOL || c_keycode < PASSWORD_POOL;) {
    uchar ch = getchar();
    if (validKeycode(ch) && c_keycode < PASSWORD_POOL)
      s[c_keycode++].keycode = ch;
    else if (c_modifier < PASSWORD_POOL)
      s[c_modifier++].modifier = (ch % 2);
     c_modifier++; 
  }
  
  fwrite(s, sizeof(passwordSeed), PASSWORD_POOL, out);
  
  return EXIT_SUCCESS;
}
