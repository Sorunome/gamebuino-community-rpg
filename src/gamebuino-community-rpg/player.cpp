#include "player.h"
#include <Gamebuino.h>
#include "settings.h"
#include "globals.h"
#include "util.h"
#include "graphics.h"
#include "script.h"
#include "player_sprites.h"
extern Gamebuino gb;

#define WALL_X() x+=vx;vx=-vx;
#define WALL_Y() y+=vy;vy=-vy;
#define NOMOVE() x_temp=y_temp=0;

#define PX (*(((int8_t*)&x)+1))
#define PY (*(((int8_t*)&y)+1))

Player player;


typedef int8_t (*FunctionPointer_Status) (int8_t&, int8_t&);

int8_t update_idle(int8_t& x, int8_t& y) {return player.update_idle(x,y);}
int8_t update_fall_wait(int8_t& x, int8_t& y) {return player.update_fall_wait(x,y);}
int8_t update_fall(int8_t& x, int8_t& y) {return player.update_fall(x,y);}
int8_t update_sword(int8_t& x, int8_t& y) {return player.update_sword(x,y);}
int8_t update_sword_spin(int8_t& x, int8_t& y) {return player.update_sword_spin(x,y);}
int8_t update_sword_spin_do_wait(int8_t& x, int8_t& y) {return player.update_sword_spin_do_wait(x,y);}
int8_t update_sword_spin_do(int8_t& x, int8_t& y) {return player.update_sword_spin_do(x,y);}
const FunctionPointer_Status PROGMEM player_status[7] = {
	update_idle,
	update_fall_wait,
	update_fall,
	update_sword,
	update_sword_spin,
	update_sword_spin_do_wait,
	update_sword_spin_do,
};


int8_t Player::calcSwordX(bool cond) {
	return PX + (cond?6:8)*(direction==0) - (cond?6:8)*(direction==2) + (cond*(6*(direction==1) - 6*(direction==3)));
}

int8_t Player::calcSwordY(bool cond) {
	return PY + (cond?6:8)*(direction==3) - (cond?6:8)*(direction==1) + (cond*(6*(direction==0) - 6*(direction==2)));
}

int8_t Player::update_idle(int8_t& x_temp, int8_t& y_temp) {
	if (gb.buttons.pressed(BTN_A)) {
		// A is pressed! determine what to do
		x_temp = (!direction)-(direction==2);
		y_temp = (direction==3)-(direction==1);
		
		switch (getWalkInfo(PX+2, PY+4, 4, 4, x_temp, y_temp)) {
			case FLAG_TILE_MANUAL_SCRIPT:
				if (x_temp > 0) {
					x_temp = 4;
				}
				if (y_temp > 0) {
					y_temp = 4;
				}
				script.loadInTilemap(((PX+2+x_temp) / 8) + (((PY+4+y_temp) / 8)*TILEMAP_WIDTH));
				return script.run();
			default:
				counter = 0;
				animation = 2;
				status = Player_Status::sword_spin;
		}
		return -1;
	}
	
	// only in idle update direction&animation
	if (x_temp || y_temp) { // if either of them changed move the player
		animation = millis() / ANIMATION_FREQUENCY % 2; // update the animation

		// calculate the direction
		direction = (1+x_temp)*(x_temp != 0);
		direction = (2+y_temp)*(y_temp != 0 || direction == 0);
	}
	return -1;
}

int8_t Player::update_fall_wait(int8_t& x_temp, int8_t& y_temp) {
	if(y_temp != 1){
		status = Player_Status::idle;
		return -1;
	}
	if(counter++ > 10){
		status = Player_Status::fall;
	}
	NOMOVE();
	return 1;
}

int8_t Player::update_fall(int8_t& x_temp, int8_t& y_temp) {
	y += 0x100; // we fall DOWN
	if (PY > (TILEMAP_HEIGHT*8) - 8) {
		// we need to load a new map!
		currentMap += DATFILE_TILEMAPS_WIDTH;
		y = (-4)*0x100;
		return 0;
	}
	// check if we landed on ground
	if (getWalkInfo_y(PX+2, PY, 4, 4, 1) <= FLAG_TILE_WALKABLE && getWalkInfo_y(PX+2, PY+4, 4, 4, 1) <= FLAG_TILE_WALKABLE) {
		status = Player_Status::idle;
	}
	focusCam();
	return 1; // we don't want to allow moving!
}

int8_t Player::update_sword(int8_t& x_temp, int8_t& y_temp) {
	return update_sword_spin(x_temp, y_temp);
}

int8_t Player::update_sword_spin(int8_t& x_temp, int8_t& y_temp) {
	if(!gb.buttons.pressed(BTN_A)){
		status = Player_Status::sword;
	}
	if(++counter >= 20){
		counter = 0;
		if (status == Player_Status::sword_spin) {
			status = Player_Status::sword_spin_do_wait;
		} else {
			animation = 0;
			status = Player_Status::idle;
		}
	}
	NOMOVE();
	return -1;
}

int8_t Player::update_sword_spin_do_wait(int8_t& x_temp, int8_t& y_temp) {
	if (gb.buttons.pressed(BTN_A)) {
		if (++counter >= 12+6) {
			counter = 12;
		}
	} else {
		if (counter >= 12) {
			counter = 0;
			status = Player_Status::sword_spin_do;
		} else {
			counter = 0;
			animation = 0;
			status = Player_Status::idle;
		}
	}
	return -1;
}

int8_t Player::update_sword_spin_do(int8_t& x_temp, int8_t& y_temp) {
	if (!(counter++%6)) {
		direction++;
		direction %= 4;
	}
	if (counter >= 4*6) {
		animation = 0;
		status = Player_Status::idle;
	}
	NOMOVE();
	return -1;
}


bool Player::update() {
	// get the x and y offsets (the way we want to move)
	int8_t x_temp = -gb.buttons.pressed(BTN_LEFT) + gb.buttons.pressed(BTN_RIGHT);
	int8_t y_temp = -gb.buttons.pressed(BTN_UP) + gb.buttons.pressed(BTN_DOWN);
	
	int8_t res = ((FunctionPointer_Status) pgm_read_word(&player_status[(uint8_t)status]))(x_temp, y_temp);
	if (res != -1) {
		return res != 0;
	}
	
	// time to calculate the move
	// time to update the actual positions based on acceleration / velocity
	if (x_temp) {
		vx += x_temp*acceleration;
		vx += x_temp; // we actually want one more!
		if (vx > 0x100) {
			vx = 0x100;
		}
		if (vx < -0x100) {
			vx = -0x100;
		}
	}
	// we want to decellerate if we haven't moved or if we are moving in the opposite direction as velocity, else it would make no sense trying to stop like that
	if (!x_temp || (x_temp > 0 && vx < 0) || (x_temp < 0 && vx > 0)) {
		if (vx > 0) {
			vx -= acceleration;
			vx--;
			if(vx < 0){
				vx = 0;
			}
		} else if (vx < 0) {
			vx += acceleration;
			vx++;
			if (vx > 0) {
				vx = 0;
			}
		}
	}
	
	
	if (vx) {
		if (vx > 0) {
			x_temp = 1;
		} else {
			x_temp = -1;
		}
		switch(getWalkInfo_x(PX+2, PY+4, 4, 4, x_temp)) {
			case FLAG_TILE_WALKABLE:
				acceleration = 0xFF;
				break;
			case FLAG_TILE_WATER:
				acceleration = 10;
				break;
			case FLAG_TILE_ICE:
				acceleration = 4;
				break;
			case FLAG_TILE_SCREENEND:
				// we walked off the screen! We need to switch tilemaps!
				if (x_temp < 0) {
					currentMap--;
					x = ((TILEMAP_WIDTH*8) - 8 + 2)*0x100;
				} else {
					currentMap++;
					x = (-2)*0x100;
				}
				return false; // no way we are just continuing the loaded tilemap, we need to load a new one!
			case FLAG_TILE_SCRIPT:
				if (x_temp > 0) {
					x_temp = 4;
				}
				script.loadInTilemap(((PX+2+x_temp) / 8) + ((PY+4) / 8)*TILEMAP_WIDTH);
				if (!script.run()) {
					vx = vy = 0;
					return true;
				}
				break;
			default:
				// we encountered a wall
				WALL_X();
		}
		x += vx;
	}
	
	// now the y-direction
	if (y_temp) {
		vy += y_temp*acceleration;
		vy += y_temp; // we actually want one more!
		if (vy > 0x100) {
			vy = 0x100;
		}
		if(vy < -0x100){
			vy = -0x100;
		}
	}
	// we want to decellerate if we haven't moved or if we are moving in the opposite direction as velocity, else it would make no sense trying to stop like that
	if (!y_temp || (y_temp > 0 && vy < 0) || (y_temp < 0 && vy > 0)) {
		if (vy > 0) {
			vy -= acceleration;
			vy--;
			if (vy < 0) {
				vy = 0;
			}
		} else if (vy < 0) {
			vy += acceleration;
			vy++;
			if (vy > 0) {
				vy = 0;
			}
		}
	}


	if (vy) {
		if (vy > 0) {
			y_temp = 1;
		} else {
			y_temp = -1;
		}
		switch(getWalkInfo_y(PX+2, PY+4, 4, 4, y_temp)) {
			case FLAG_TILE_WALKABLE:
				acceleration = 0xFF;
				break;
			case FLAG_TILE_WATER:
				acceleration = 10;
				break;
			case FLAG_TILE_ICE:
				acceleration = 4;
				break;
			case FLAG_TILE_SCREENEND:
				// we walked off the screen! We need to switch tilemaps!
				if (y_temp < 0) {
					currentMap -= DATFILE_TILEMAPS_WIDTH;
					y = ((TILEMAP_HEIGHT*8) - 8)*0x100;
				} else {
					currentMap += DATFILE_TILEMAPS_WIDTH;
					y = (-4)*0x100;
				}
				return false;
			case FLAG_TILE_FALL:
				if (status == Player_Status::idle) {
					counter = 0;
					status = Player_Status::fall_wait;
				}
				WALL_Y(); // else we act like a wall
				break;
			case FLAG_TILE_SCRIPT:
				if (y_temp > 0) {
					y_temp = 4;
				}
				script.loadInTilemap(((PX+2) / 8) + (((PY+4+y_temp) / 8)*TILEMAP_WIDTH));
				if (!script.run()) {
					vx = vy = 0;
					return true;
				}
				break;
			default:
				// we encountered a wall
				WALL_Y();
		}
		y += vy;
	}
	
	focusCam();
	
	return true;
}

void Player::focusCam() {
	// this just focuses the cam onto the player
	moveCam(PX - (LCDWIDTH / 2), PY - (LCDHEIGHT / 2));
}

void Player::draw() {
	switch(status){
		case Player_Status::sword:
		case Player_Status::sword_spin:
			memcpy_P(tmpsprite, charset_sword+(direction*24*2+(counter<5)*16*2), 16);
			sprite_masked(tmpsprite,calcSwordX(counter<5),calcSwordY(counter<5));
			break;
		case Player_Status::sword_spin_do_wait:
			memcpy_P(tmpsprite,charset_sword+(direction*24*2+(counter>=12)*((counter%6) >= 3)*16),16);
			sprite_masked(tmpsprite,
				PX + 8*(direction==0) - 8*(direction==2),
				PY + 8*(direction==3) - 8*(direction==1)
			);
			break;
		case Player_Status::sword_spin_do:
			memcpy_P(tmpsprite,charset_sword+(direction*24*2+((counter%6)<3)*16*2),16);
			sprite_masked(tmpsprite,calcSwordX((counter%6)<3),calcSwordY((counter%6)<3));
			break;
	}
	memcpy_P(tmpsprite, charset_player+(direction*24*2+(animation)*8*2), 16);
	sprite_masked(tmpsprite, PX, PY);
}
