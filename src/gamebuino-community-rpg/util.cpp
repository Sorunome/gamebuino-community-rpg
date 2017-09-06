#include "util.h"
#include "globals.h"
#include "settings.h"
#include "enemies.h"
#include "script.h"
#include "graphics.h"
#include "player.h"
#include <Gamebuino.h>
extern Gamebuino gb;

byte getWalkInfo(int8_t x, int8_t y, byte w, byte h, int8_t dx, int8_t dy) {
	if (dx) {
		return getWalkInfo_x(x,y,w,h,dx);
	}
	return getWalkInfo_y(x,y,w,h,dy);
}

/*
	loadSong

	this will load a sound and play it in the background

	takes: number of the song, starting with zero
	returns: none
*/
void loadSong(uint16_t num) {
	gb.sound.stopTrack(0); // first stop the currently playing stuff
	gb.sound.stopTrack(1);
#if ENABLE_SOUND
	// a soundbuffer is 1024 bytes while the screenbuffer is only 512 bytes, thus we preform two reads
	soundfile.read(screenbuffer, num*1024, 512);
	write_flash_page((const char*)SOUNDBUFFER_OFFSET, screenbuffer); // one flash sector is 128 bytes (4*128 = 512)
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128), screenbuffer+128);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*2), screenbuffer+128*2);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*3), screenbuffer+128*3);
	soundfile.read(screenbuffer, num*1024+512, 512);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*4), screenbuffer);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*5), screenbuffer+128);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*6), screenbuffer+128*2);
	write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*7), screenbuffer+128*3);

	gb.sound.playTrack(0); // now start the tracks again, no need to specify where the buffer is as this is hardcoded
	gb.sound.playTrack(1);
	//gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET),0);
	//gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET + 40),1);
#endif
}


/*
	addEnemy

	adds an enemy to the current map
	TODO: add a way to tell which kind of enemy

	takes: x,y
	returns: index of enemy
*/
byte addEnemy(byte type, byte x, byte y) {
	for (byte i = 0; i < MAX_NUM_ENEMIES;i++) {
		if (enemies[i] == NULL) {
			enemies[i] = new Enemy(type,x,y);
			return i;
		}
	}
	return 0xFF;
}


/*
	exitTilemap

	This gets called when exiting a tilemap to check if there are any scripts we need to execute

	takes: none
	returns: true if continue normally (load the next tilemap), false if it already loaded a tilemap
*/
bool exitTilemap() {
	if(!mapAddress){ // if there is no previous map just don't do anything
		return true;
	}
	uint8_t i;
	datfile.read(&i, mapAddress, 1);
	if(i&1){ // we have an exit script!
		datfile.read((byte*)&(script.cursor), mapAddress + DATFILE_TILEMAP_SIZE + 4, 4);
		return script.run();
	}
	return true;
}


/*
	loadTilemap

	This will load a tilemap, populate the tileset and the tilemap buffer
	TODO: scripts on loading which spawn enemies etc.

	takes: number of the tilemap
	returns: none
*/
void loadTilemap(uint8_t num) {
	uint8_t i;
	for (i = 0; i < MAX_NUM_ENEMIES; i++){ // delete all the enemies
		delete enemies[i];
		enemies[i] = NULL;
	}
	firstAnimatedSprite = TILEMAPS_SPRITESPACE; // time to reset the animated counter

	datfile.read((byte*)&mapAddress, DATFILE_START_TILEMAPLUT + (currentWorld*4), 4); // fetch the address of the LUT for the current world

	// now we search for the address of the tilemap
	datfile.read(screenbuffer, mapAddress, 256);
	i = 0;
	while(true) {
		if (screenbuffer[i] == num) { // we found the map, now store the address
			memcpy(&mapAddress, &screenbuffer[i+1], 4);
			break;
		}
		if (i == 255) { // we need to load more stuff into the buffer
			mapAddress += 255;
			i = 0;
			datfile.read(screenbuffer, mapAddress, 256);
			continue;
		}
		i += 5; // 5 = 1 + 4; 4 - address, 1 - tilemap num
	}

	// get the expanded tilemap into the screebnuffer
	datfile.read(screenbuffer, mapAddress, DATFILE_TILEMAP_SIZE);

	// this buffer is used to keep track of which sprites were already loaded and which new ID they have, 0xFF means an empty slot
	uint16_t* have_sprite_buf = (uint16_t*)screenbuffer + DATFILE_TILEMAP_SIZE;
	memset(have_sprite_buf, 0xFF, 2*TILEMAPS_SPRITESPACE);

	// just give it a different name for easier understanding
	uint16_t* tmptilemapbuf = (uint16_t*)(screenbuffer + DATFILE_TILEMAPS_HEADER_SIZE);

	// we cannot use tmpsprite for this as we need a 17-byte buffer (1-byte for walking information), fortunatly the screen buffer is large enough
	byte* tmpsprite = (uint8_t*)have_sprite_buf + (2*TILEMAPS_SPRITESPACE);

	for (i = 0; i < TILEMAP_SIZE; i ++) {

		if (tmptilemapbuf[i] >= SPRITES_FIRST_ANIMATED) {
			// the tile we'r looking at is animated, so we search from the back of the buffer if we have it already and if not import it
			for (num = TILEMAPS_SPRITESPACE-2; num >= 0; num -= 2) {
				if (have_sprite_buf[num] == tmptilemapbuf[i]) { // we don't need it anymore!
					tilemap[i] = num;
					break;
				}
				if (have_sprite_buf[num] == 0xffff) { // apparently the sprite wasn't loaded yet, time to import it!
					have_sprite_buf[num] = tmptilemapbuf[i]; // add it to the sprites we have
					// read the data from the sd card
					datfile.read(tmpsprite, ((tmptilemapbuf[i] - SPRITES_FIRST_ANIMATED)*17) + DATFILE_START_SPRITES + (SPRITES_FIRST_ANIMATED*9), 17);
					walksprites[num] = tmpsprite[0]; // set the walking data
					memcpy(num*8 + tileset, tmpsprite+1, 16); // set the sprite data
					tilemap[i] = num;
					firstAnimatedSprite = num; // no need to perform additional logic on this, as due to how the logic will work this will always be smaller than the previous one.
					break;
				}
			}
		} else {
			// the tile we'r looking at is static, so we search from the front of the buffer if we have it loaded already
			for (num = 0; num < TILEMAPS_SPRITESPACE; num++) {
				if (have_sprite_buf[num] == tmptilemapbuf[i]) { // we don't need it anymore!
					tilemap[i] = num;
					break;
				}
				if (have_sprite_buf[num] == 0xffff) { // we need to load the sprite!
					have_sprite_buf[num] = tmptilemapbuf[i];
					datfile.read(tmpsprite, (tmptilemapbuf[i]*9) + DATFILE_START_SPRITES, 9);
					walksprites[num] = tmpsprite[0];
					memcpy(num*8 + tileset, tmpsprite + 1, 8);
					tilemap[i] = num;
					break;
				}
			}
		}
	}
	if (screenbuffer[0] & 0x02) { // we have the header byte in here!
		// we need to run a start script!

		// we are using a custom script object as this may be called within another script, resulting in the cursor etc. being off
		Script tempscript;
		datfile.read((byte*)&(tempscript.cursor), mapAddress + DATFILE_TILEMAP_SIZE, 4);
		tempscript.run();
	}
}


/*
	getTileAt

	returns the tile in the tilemap at pixel x/y coordinates

	takes: x,y
	returns: sprite id
*/
byte getTileAt(int8_t x, int8_t y) {
	return tilemap[(x / 8) + ((y/8) * TILEMAP_WIDTH)];
}
/*
	getFlagAt

	returns the walking flag at pixel x/y coordinates

	takes: x,y
	returns: walking flag
*/
byte getFlagAt(int8_t x, int8_t y) {
	byte tmp = walksprites[getTileAt(x, y)];
	return 1 + (((tmp >> ((x%8 < 4) + 2*(y%8 < 4))) & 0x01)*((tmp >> 4)+1));
}
/*
	getWalkInfo

	fetches the info if you can walk into a certain thing and which flags are present there

	takes: x,y,width,height,difference in x,difference in y
	returns: walking flag
*/
byte getWalkInfo_x(int8_t x, int8_t y, byte w, byte h, int8_t dx) {
	if (!dx) {
		return FLAG_TILE_WALKABLE;
	}
	byte tmp;
	w--; // this makes our lives easier, as we start counting at zero then
	h--;

	x += dx;
	if (dx > 0) { // we are moving right
		if (x+w >= TILEMAP_WIDTH*8) {
			return FLAG_TILE_SCREENEND;
		}
		tmp = getFlagAt(x+w, y);
		if (tmp <= FLAG_TILE_WALKABLE) {
			return getFlagAt(x+w, y+h);
		}
		return tmp;
	}
	if (x < 0) { // we are moving left
		return FLAG_TILE_SCREENEND;
	}
	tmp = getFlagAt(x, y);
	if (tmp <= FLAG_TILE_WALKABLE) {
		return getFlagAt(x, y+h);
	}
	return tmp;
}
byte getWalkInfo_y(int8_t x, int8_t y, byte w, byte h, int8_t dy) {
	if (!dy) {
		return FLAG_TILE_WALKABLE;
	}
	byte tmp;
	w--; // this makes our lives easier, as we start counting at zero then
	h--;

	y += dy;
	if (dy > 0) { // we are moving down
		if (y+h >= TILEMAP_HEIGHT*8) {
			return FLAG_TILE_SCREENEND;
		}
		tmp = getFlagAt(x, y+h);
		if (tmp <= FLAG_TILE_WALKABLE) {
			return getFlagAt(x+w, y+h);
		}
		return tmp;
	}
	if (y < 0) { // we are moving up
		return FLAG_TILE_SCREENEND;
	}
	tmp = getFlagAt(x, y);
	if (tmp <= FLAG_TILE_WALKABLE) {
		return getFlagAt(x+w, y);
	}
	return tmp;
}


/*
	moveCam

	moves the camera to certain coordinates, paying attention that the cam doesn't go off-bounds

	takes: x,y
	returns: none
*/
void moveCam(int8_t x, int8_t y){
	camX = x;
	camY = y;
	if (x < 0) {
		camX = 0;
	}
	if (x > (TILEMAP_WIDTH*8) - LCDWIDTH) {
		camX = (TILEMAP_WIDTH*8) - LCDWIDTH;
	}

	if (y < 0) {
		camY = 0;
	}
	if (y > (TILEMAP_HEIGHT*8) - LCDHEIGHT) {
		camY = (TILEMAP_HEIGHT*8) - LCDHEIGHT;
	}
}


/*
	drawScreen

	clears the screen and draws it (tilemap, player, enemies). Call gb.display.update() manually!

	takes: none
	returns: none
*/
void drawScreen(void){
	gb.display.clear();
	drawTilemap();
	player.draw();
	for (byte i = 0; i < MAX_NUM_ENEMIES; i++) {
		if (enemies[i]!=NULL) {
			enemies[i]->draw();
		}
	}
}
