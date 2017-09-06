#include "defines.h"
#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include <GB_Fat.h>

Gamebuino gb;

#include "globals.h"
#include "util.h"
#include "player.h"
#include "enemies.h"
#include "graphics.h"


void setup(){
	Serial.begin(19200);
	while(!Serial);
	Serial.println("Starting game...");
	GB_Fat sd;
	gb.begin();

	// no need as we save ram by modifying the library!
	//gb.setFrameRate(40); // it just looked smoother IMO

	// TODO: Different displaying stuff while loading the card
	gb.display.clear();
	gb.display.println(F("loading card..."));
	gb.display.update();

	if (sd.init(screenbuffer, SPISPEED_VERYHIGH) != NO_ERROR) {
		gb.display.clear();
		gb.display.print(F("SD card not found."));
		gb.display.update();
		while(1);
	}
	gb.display.clear();
	gb.display.println(F("Searching for file card..."));
	gb.display.update();

	soundfile = sd.open("SOUND.DAT", screenbuffer);
	if (!soundfile.exists()) {
		gb.display.clear();
		gb.display.print(F("Couldn't open sound file."));
		gb.display.update();
		while(1);
	}
	datfile = sd.open("DATA.DAT", screenbuffer);
	if (!datfile.exists()) {
		gb.display.clear();
		gb.display.print(F("Couldn't open file."));
		gb.display.update();
		while(1);
	}

	loadTilemap(currentMap);
	gb.display.clear();
	gb.display.println(F("SD card found."));
	gb.display.update();

	//gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 0);
	//gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 1);

	loadSong(0);
	//gb.display.setContrast(gb.display.contrast);
}


void loop() {
	if (!gb.update()) {
		return;
	}
	if (player.update()) {
		drawTilemap();
		player.draw();
		for (byte i = 0;i < MAX_NUM_ENEMIES;i++) {
			if(enemies[i]!=NULL){
				enemies[i]->update();
				enemies[i]->draw();
			}
		}
	} else if (exitTilemap()) {
		// if we are here the player left a tilemap, time to load a different one! (currentMap already got updated)
		loadTilemap(currentMap);
		player.focusCam();
	}
}
