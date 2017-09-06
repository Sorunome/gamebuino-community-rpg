#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <Arduino.h>
#include "settings.h"
#include <GB_Fat.h>

extern byte tileset[];
extern byte tmpsprite[];
extern byte walksprites[];
extern byte currentMap;
extern byte currentWorld;
extern uint32_t mapAddress;
extern byte* screenbuffer;

extern byte camX;
extern byte camY;
extern byte tilemap[];
extern byte firstAnimatedSprite;
extern GB_File datfile;
extern GB_File soundfile;

#endif // _GLOBALS_H_
