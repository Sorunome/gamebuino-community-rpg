// TODO: Think of a way to differentiate between multiple enemies. Maybe just use a large switch?

class Enemy {
	byte type;
	byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
	byte state; // 0 = walking right, 1 = walking up, 2 = walking left, 3 = walking down, 4 = wait, 5 = idle
	byte counter;
	public:
		byte x,y;
		Enemy(byte t,byte cx,byte cy){
			type = t;
			x = cx;
			y = cy;

			state = 5;
		}
		void update(void){
			if(state == 5){ // standing still

				state = random(0,5); // pick a random direction to walk to
				if(state < 4){
					direction = state;
				}
				counter = random(1*8,8*8); // for a random amount of time
				return;
			}
			if(!counter--){
				state = 5;
				return;
			}
			switch(state){
				case 0:
					if(getWalkInfo_x(x+2,y+4,4,4,1) != FLAG_TILE_WALKABLE){
						state = 5;
						return;
					}
					x++;
					break;
				case 1:
					if(getWalkInfo_y(x+2,y+4,4,4,-1) != FLAG_TILE_WALKABLE){
						state = 5;
						return;
					}
					y--;
					break;
				case 2:
					if(getWalkInfo_x(x+2,y+4,4,4,-1) != FLAG_TILE_WALKABLE){
						state = 5;
						return;
					}
					x--;
					break;
				case 3:
					if(getWalkInfo_y(x+2,y+4,4,4,1) != FLAG_TILE_WALKABLE){
						state = 5;
						return;
					}
					y++;
					break;
			}
		}
		void draw(void){
			switch(type){
				case 0:
					memcpy_P(tmpsprite,charset_enemy,16); // copy the sprite to RAM and display it
					sprite_masked(tmpsprite, x, y);
					break;
			}
		}
};
Enemy *enemies[MAX_NUM_ENEMIES];


/*
	addEnemy

	adds an enemy to the current map
	TODO: add a way to tell which kind of enemy

	takes: x,y
	returns: index of enemy
*/
byte addEnemy(byte type,byte x = 0,byte y = 0){
	for(byte i = 0;i < MAX_NUM_ENEMIES;i++){
		if(enemies[i] == NULL){
			enemies[i] = new Enemy(type,x,y);
			return i;
		}
	}
	return -1;
}
