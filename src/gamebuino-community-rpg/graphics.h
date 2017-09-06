#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <Arduino.h>
void sprite_masked(byte data[], byte xx, byte yy);
void sprite_xor(byte data[], byte xx, byte yy);
void drawTilemap();

#endif // _GRAPHICS_H_
