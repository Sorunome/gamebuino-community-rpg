#include "globals.h"
#include <GB_Fat.h>
#include <Gamebuino.h>

byte tileset[8*TILEMAPS_SPRITESPACE]; // this holds the current tileset used for the current tilemap
byte tmpsprite[16]; // temporary sprite buffer used for displaying sprites from PROGMEM
byte walksprites[TILEMAPS_SPRITESPACE]; // this buffer holds the data what is walkable and what isn't
byte currentMap = TILEMAPS_DEFAULTMAP; // the id of the current tilemap
byte currentWorld = 0; // the id of the current world
uint32_t mapAddress = 0; // the address to the tilemap on the sd card

extern Gamebuino gb;
byte* screenbuffer = gb.display.getBuffer(); // allows for easier access, we use that buffer a lot all throughout the place


byte camX = 0; // camera x position
byte camY = 0; // camera y position
byte tilemap[TILEMAP_SIZE]; // the buffer of the tilemap
byte firstAnimatedSprite; // the marker for when the tiles are animated
GB_File datfile; // this is the main file, holding a whole bunch of information (sprites, tilemaps, scripts etc.)
GB_File soundfile; // this file will hold the sound
