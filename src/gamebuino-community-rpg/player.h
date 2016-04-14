#define WALL_X() x+=vx;vx=-vx;
#define WALL_Y() y+=vy;vy=-vy;

class Player {
	byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
	byte animation = 0;
	byte counter = 0;
	byte status = PLAYER_STATUS_IDLE;
	public:
		int16_t x=16*0x100,y=16*0x100;
		int8_t* px=((int8_t*)&x)+1;
		int8_t* py=((int8_t*)&y)+1;
		int16_t vx = 0;
		int16_t vy = 0;
		uint8_t acceleration = 0xFF;
		bool update(){
			int8_t x_temp = 0;
			int8_t y_temp = 0;
			switch(status){
				case PLAYER_STATUS_IDLE:
					if(gb.buttons.pressed(BTN_A)){
						// A is pressed! determine what to do
						x_temp = (!direction)-(direction==2);
						y_temp = (direction==3)-(direction==1);

						if(getWalkInfo((*px)+2,(*py)+4,4,4,x_temp,y_temp)==FLAG_TILE_MANUAL_SCRIPT){
							// we need to run a script
							if(x_temp > 0){
								x_temp = 4;
							}
							if(y_temp > 0){
								y_temp = 4;
							}
							script.loadInTilemap((((*px)+2+x_temp) / 8) + ((((*py)+4+y_temp) / 8)*TILEMAP_WIDTH));
							return script.run();
						}
						break;
					}

					// get the x and y offsets
					x_temp = -gb.buttons.pressed(BTN_LEFT)+gb.buttons.pressed(BTN_RIGHT);
					y_temp = -gb.buttons.pressed(BTN_UP)+gb.buttons.pressed(BTN_DOWN);

					if(x_temp || y_temp){ // if either of them changed move the player
						animation = millis()/ANIMATION_FREQUENCY%2; // update the animation

						// calculate the direction
						direction = (1+x_temp)*(x_temp != 0);
						direction = (2+y_temp)*(y_temp != 0 || direction == 0);
					}

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
								counter = 0;
								break;
							case FLAG_TILE_WATER:
								acceleration = 10;
								counter = 0;
								break;
							case FLAG_TILE_ICE:
								acceleration = 4;
								counter = 0;
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
								counter = 0;
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
					}
					x += vx;

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
								counter = 0;
								break;
							case FLAG_TILE_WATER:
								acceleration = 10;
								counter = 0;
								break;
							case FLAG_TILE_ICE:
								acceleration = 4;
								counter = 0;
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
								counter = 0;
								return false;
							case FLAG_TILE_FALL:
								if(y_temp > 0){
									if(counter++ > 10){
										counter = 0;
										status = PLAYER_STATUS_FALL;
									}
									return true; // we don't want the player to actually move!
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
					}
					y += vy;
					focusCam();
					break;
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
					//break; // no need as it's the last one in the case-block
			}
			return true;
		}
		void focusCam(){
			// this just focuses the cam onto the player
			moveCam((*px) - (LCDWIDTH / 2), (*py) - (LCDHEIGHT / 2));
		}
		void draw(){
			memcpy_P(tmpsprite,charset_player+(direction*24*2+animation*8*2),16);
			sprite_masked(tmpsprite, *px, *py);
		}
};
Player player;
