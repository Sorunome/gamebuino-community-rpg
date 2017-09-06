// TODO: Think of a way to differentiate between multiple enemies. Maybe just use a large switch?
#include "enemies.h"
#include "settings.h"
#include "globals.h"
#include "graphics.h"
#include "util.h"



const byte charset_enemy[] PROGMEM = {
	0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF
};

Enemy::Enemy(byte t, byte cx, byte cy) {
	type = t;
	x = cx;
	y = cy;

	state = 5;
}

void Enemy::update() {
	if (state == 5) { // standing still

		state = random(0, 5); // pick a random direction to walk to
		if (state < 4) {
			direction = state;
		}
		counter = random(1*8, 8*8); // for a random amount of time
		return;
	}
	if (!counter--) {
		state = 5;
		return;
	}
	switch(state) {
		case 0:
			if (getWalkInfo_x(x+2, y+4, 4, 4, 1) != FLAG_TILE_WALKABLE) {
				state = 5;
				return;
			}
			x++;
			break;
		case 1:
			if (getWalkInfo_y(x+2, y+4, 4, 4, -1) != FLAG_TILE_WALKABLE) {
				state = 5;
				return;
			}
			y--;
			break;
		case 2:
			if (getWalkInfo_x(x+2, y+4, 4, 4, -1) != FLAG_TILE_WALKABLE) {
				state = 5;
				return;
			}
			x--;
			break;
		case 3:
			if (getWalkInfo_y(x+2, y+4, 4, 4, 1) != FLAG_TILE_WALKABLE) {
				state = 5;
				return;
			}
			y++;
			break;
	}
}

void Enemy::draw() {
	switch(type){
		case 0:
			memcpy_P(tmpsprite, charset_enemy, 16); // copy the sprite to RAM and display it
			sprite_masked(tmpsprite, x, y);
			break;
	}
}

Enemy *enemies[MAX_NUM_ENEMIES];
