#define PLAYER_STATUS_IDLE 0
#define PLAYER_STATUS_FALL_WAIT 1
#define PLAYER_STATUS_FALL 2
#define PLAYER_STATUS_SWORD 3
#define PLAYER_STATUS_SWORD_SPIN 4
#define PLAYER_STATUS_SWORD_SPIN_DO_WAIT 5
#define PLAYER_STATUS_SWORD_SPIN_DO 6

#define WALL_X() x+=vx;vx=-vx;
#define WALL_Y() y+=vy;vy=-vy;
#define NOMOVE() x_temp=y_temp=0;

class Player {
	byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
	byte animation = 0;
	byte counter = 0;
	byte status = PLAYER_STATUS_IDLE;
	int8_t calcSwordX(bool cond){
		return *px + (cond?6:8)*(direction==0) - (cond?6:8)*(direction==2) + (cond*(6*(direction==1) - 6*(direction==3)));
	}
	int8_t calcSwordY(bool cond){
		return *py + (cond?6:8)*(direction==3) - (cond?6:8)*(direction==1) + (cond*(6*(direction==0) - 6*(direction==2)));
	}
	int8_t x_temp,y_temp;
	public:
		int16_t x=16*0x100,y=16*0x100;
		int8_t* px=((int8_t*)&x)+1;
		int8_t* py=((int8_t*)&y)+1;
		int16_t vx = 0;
		int16_t vy = 0;
		uint8_t acceleration = 0xFF;
		bool update(){
			// get the x and y offsets (the way we want to move)
			x_temp = -gb.buttons.pressed(BTN_LEFT)+gb.buttons.pressed(BTN_RIGHT);
			y_temp = -gb.buttons.pressed(BTN_UP)+gb.buttons.pressed(BTN_DOWN);
			
			switch(status){
				case PLAYER_STATUS_IDLE:
					if(gb.buttons.pressed(BTN_A)){
						// A is pressed! determine what to do
						x_temp = (!direction)-(direction==2);
						y_temp = (direction==3)-(direction==1);
						
						switch (getWalkInfo((*px)+2,(*py)+4,4,4,x_temp,y_temp)) {
							case FLAG_TILE_MANUAL_SCRIPT:
								if(x_temp > 0){
									x_temp = 4;
								}
								if(y_temp > 0){
									y_temp = 4;
								}
								script.loadInTilemap((((*px)+2+x_temp) / 8) + ((((*py)+4+y_temp) / 8)*TILEMAP_WIDTH));
								return script.run();
							default:
								counter = 0;
								animation = 2;
								status = PLAYER_STATUS_SWORD_SPIN;
						}
						break;
					}
					
					// only in idle update direction&animation
					if(x_temp || y_temp){ // if either of them changed move the player
						animation = millis()/ANIMATION_FREQUENCY%2; // update the animation

						// calculate the direction
						direction = (1+x_temp)*(x_temp != 0);
						direction = (2+y_temp)*(y_temp != 0 || direction == 0);
					}
					break;
				case PLAYER_STATUS_FALL_WAIT:
					if(y_temp != 1){
						status = PLAYER_STATUS_IDLE;
						break;
					}
					if(counter++ > 10){
						status = PLAYER_STATUS_FALL;
					}
					NOMOVE();
					return true;
				case PLAYER_STATUS_FALL:
					(*py)++; // we fall DOWN
					if((*py) > (TILEMAP_HEIGHT*8) - 8){
						// we need to load a new map!
						currentMap += DATFILE_TILEMAPS_WIDTH;
						y = (-4)*0x100;
						return false;
					}
					// check if we landed on ground
					if(getWalkInfo_y((*px)+2,*py,4,4,1) <= FLAG_TILE_WALKABLE && getWalkInfo_y((*px)+2,(*py)+4,4,4,1) <= FLAG_TILE_WALKABLE){
						status = PLAYER_STATUS_IDLE;
					}
					focusCam();
					return true; // we don't want to allow moving!
				case PLAYER_STATUS_SWORD:
				case PLAYER_STATUS_SWORD_SPIN:
					if(!gb.buttons.pressed(BTN_A)){
						status = PLAYER_STATUS_SWORD;
					}
					if(++counter >= 20){
						counter = 0;
						if(status == PLAYER_STATUS_SWORD_SPIN){
							status = PLAYER_STATUS_SWORD_SPIN_DO_WAIT;
						}else{
							animation = 0;
							status = PLAYER_STATUS_IDLE;
						}
					}
					NOMOVE();
					break;
				case PLAYER_STATUS_SWORD_SPIN_DO_WAIT:
					if(gb.buttons.pressed(BTN_A)){
						if(++counter >= 12+6){
							counter = 12;
						}
					}else{
						if(counter >= 12){
							counter = 0;
							status = PLAYER_STATUS_SWORD_SPIN_DO;
						}else{
							counter = 0;
							animation = 0;
							status = PLAYER_STATUS_IDLE;
						}
					}
					break;
				case PLAYER_STATUS_SWORD_SPIN_DO:
					if(!(counter++%6)){
						direction++;
						direction %= 4;
					}
					if(counter >= 4*6){
						animation = 0;
						status = PLAYER_STATUS_IDLE;
					}
					NOMOVE();
					break;
			}
			
			// time to calculate the move
			// time to update the actual positions based on acceleration / velocity
			if(x_temp){
				vx += x_temp*acceleration;
				vx += x_temp; // we actually want one more!
				if(vx > 0x100){
					vx = 0x100;
				}
				if(vx < -0x100){
					vx = -0x100;
				}
			}
			// we want to decellerate if we haven't moved or if we are moving in the opposite direction as velocity, else it would make no sense trying to stop like that
			if(!x_temp || (x_temp > 0 && vx < 0) || (x_temp < 0 && vx > 0)){
				if(vx > 0){
					vx -= acceleration;
					vx--;
					if(vx < 0){
						vx = 0;
					}
				}else if(vx < 0){
					vx += acceleration;
					vx++;
					if(vx > 0){
						vx = 0;
					}
				}
			}
			
			
			if(vx){
				if(vx > 0){
					x_temp = 1;
				}else{
					x_temp = -1;
				}
				switch(getWalkInfo_x((*px)+2,(*py)+4,4,4,x_temp)){
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
						if(x_temp < 0){
							currentMap--;
							x = ((TILEMAP_WIDTH*8) - 8 + 2)*0x100;
						}else{
							currentMap++;
							x = (-2)*0x100;
						}
						return false; // no way we are just continuing the loaded tilemap, we need to load a new one!
					case FLAG_TILE_SCRIPT:
						if(x_temp > 0){
							x_temp = 4;
						}
						script.loadInTilemap((((*px)+2+x_temp) / 8) + ((((*py)+4) / 8)*TILEMAP_WIDTH));
						if(!script.run()){
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
			if(y_temp){
				vy += y_temp*acceleration;
				vy += y_temp; // we actually want one more!
				if(vy > 0x100){
					vy = 0x100;
				}
				if(vy < -0x100){
					vy = -0x100;
				}
			}
			// we want to decellerate if we haven't moved or if we are moving in the opposite direction as velocity, else it would make no sense trying to stop like that
			if(!y_temp || (y_temp > 0 && vy < 0) || (y_temp < 0 && vy > 0)){
				if(vy > 0){
					vy -= acceleration;
					vy--;
					if(vy < 0){
						vy = 0;
					}
				}else if(vy < 0){
					vy += acceleration;
					vy++;
					if(vy > 0){
						vy = 0;
					}
				}
			}


			if(vy){
				if(vy > 0){
					y_temp = 1;
				}else{
					y_temp = -1;
				}
				switch(getWalkInfo_y((*px)+2,(*py)+4,4,4,y_temp)){
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
						if(y_temp < 0){
							currentMap -= DATFILE_TILEMAPS_WIDTH;
							y = ((TILEMAP_HEIGHT*8) - 8)*0x100;
						}else{
							currentMap += DATFILE_TILEMAPS_WIDTH;
							y = (-4)*0x100;
						}
						return false;
					case FLAG_TILE_FALL:
						if(status == PLAYER_STATUS_IDLE){
							counter = 0;
							status = PLAYER_STATUS_FALL_WAIT;
						}
						WALL_Y(); // else we act like a wall
						break;
					case FLAG_TILE_SCRIPT:
						if(y_temp > 0){
							y_temp = 4;
						}
						script.loadInTilemap((((*px)+2) / 8) + ((((*py)+4+y_temp) / 8)*TILEMAP_WIDTH));
						if(!script.run()){
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
		void focusCam(){
			// this just focuses the cam onto the player
			moveCam((*px) - (LCDWIDTH / 2), (*py) - (LCDHEIGHT / 2));
		}
		void draw(){
			switch(status){
				case PLAYER_STATUS_SWORD:
				case PLAYER_STATUS_SWORD_SPIN:
					memcpy_P(tmpsprite,charset_sword+(direction*24*2+(counter<5)*16*2),16);
					sprite_masked(tmpsprite,calcSwordX(counter<5),calcSwordY(counter<5));
					break;
				case PLAYER_STATUS_SWORD_SPIN_DO_WAIT:
					memcpy_P(tmpsprite,charset_sword+(direction*24*2+(counter>=12)*((counter%6) >= 3)*16),16);
					sprite_masked(tmpsprite,
						*px + 8*(direction==0) - 8*(direction==2),
						*py + 8*(direction==3) - 8*(direction==1)
					);
					break;
				case PLAYER_STATUS_SWORD_SPIN_DO:
					memcpy_P(tmpsprite,charset_sword+(direction*24*2+((counter%6)<3)*16*2),16);
					sprite_masked(tmpsprite,calcSwordX((counter%6)<3),calcSwordY((counter%6)<3));
					break;
			}
			memcpy_P(tmpsprite,charset_player+(direction*24*2+(animation)*8*2),16);
			sprite_masked(tmpsprite, *px, *py);
		}
};
Player player;
