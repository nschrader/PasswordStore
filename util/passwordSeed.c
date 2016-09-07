/*
 * passwordSeed.c
 * 
 * This program generates a password seed to be flashed into an EEPROM
 * with SIZE bytes. Each seedbyte is generated in the form:
 * {USB-HID-Keycode} {0/1 (SMALL/CAPS)} {0}, i.e. 01100110
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 256
#define validKeycode(x) ((x >= 0x04 && x <= 0x27) || (x >= 0x2d && x <= 0x31) || (x >= 0x33 && x <= 0x38))

typedef unsigned char uchar;

#pragma pack(1)
typedef struct {
  uchar :1;
  uchar modifier:1;
  uchar keycode:6;
} passwordSeed;

int main(int argc, char *argv[]) {
  passwordSeed s[SIZE];
  FILE *out;
  
  if (argc > 1)
    out = fopen(argv[1], "w+b");
  else
    out = fopen("eeprom.bin", "w+b");
  if (out == NULL)
    return EXIT_FAILURE;
  
  memset(s, 0, sizeof(s));
  for (int c_modifier = 0, c_keycode = 0; c_modifier < SIZE || c_keycode < SIZE;) {
    uchar ch = getchar();
    if (validKeycode(ch) && c_keycode < SIZE)
      s[c_keycode++].keycode = ch;
    else if (c_modifier < SIZE)
      s[c_modifier++].modifier = (ch % 2);
     c_modifier++; 
  }
  
  fwrite(s, sizeof(passwordSeed), SIZE, out);
  
  return EXIT_SUCCESS;
}
