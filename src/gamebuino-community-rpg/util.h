#ifndef _UTIL_H_
#define _UTIL_H_

#include <Arduino.h>

void loadSong(uint16_t num);
byte getWalkInfo(int8_t x, int8_t y, byte w, byte h, int8_t dx, int8_t dy);

byte addEnemy(byte type, byte x = 0, byte y = 0);

bool exitTilemap();
void loadTilemap(uint8_t num);

byte getTileAt(int8_t x,int8_t y);
byte getFlagAt(int8_t x,int8_t y);
byte getWalkInfo_x(int8_t x, int8_t y, byte w, byte h, int8_t dx);
byte getWalkInfo_y(int8_t x, int8_t y, byte w, byte h, int8_t dy);

void moveCam(int8_t x, int8_t y);

void drawScreen();
#endif // _UTIL_H_
