#define PASSWORD_LENGTH 16
#define PASSWORD_POOL 256

#ifndef uchar
typedef unsigned char uchar;
#endif

#pragma pack(1)
typedef struct {
  uchar :1;
  uchar modifier:1;
  uchar keycode:6;
} passwordSeed;