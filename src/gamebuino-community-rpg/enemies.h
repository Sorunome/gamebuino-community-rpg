

class Enemy {
  byte x,y;
  byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
  byte state;
  byte counter;
  public:
    Enemy(byte cx,byte cy){
      x = cx*8;
      y = cy*8;
      
      state = 4;
    }
    void update(){
      if(state == 4){ // standing still
        
        state = direction = random(0,4);
        counter = random(1*8,8*8);
        return;
      }
      if(!counter--){
        state = 4;
        return;
      }
      if(state == 0){
        if(getWalkInfo(x+2,y+4,4,4,1,0) != FLAG_TILE_WALKABLE){
          state = 4;
          return;
        }
        x++;
      }
      if(state == 1){
        if(getWalkInfo(x+2,y+4,4,4,0,-1) != FLAG_TILE_WALKABLE){
          state = 4;
          return;
        }
        y--;
      }
      if(state == 2){
        if(getWalkInfo(x+2,y+4,4,4,-1,0) != FLAG_TILE_WALKABLE){
          state = 4;
          return;
        }
        x--;
      }if(state == 3){
        if(getWalkInfo(x+2,y+4,4,4,0,1) != FLAG_TILE_WALKABLE){
          state = 4;
          return;
        }
        y++;
      }
    }
    void draw(){
      memcpy_P(tmpsprite,charset_enemy,16);
      sprite_masked(tmpsprite, x, y);
    }
};
Enemy *enemies[MAX_NUM_ENEMIES];


void addEnemy(byte x,byte y){
  for(byte i = 0;i < MAX_NUM_ENEMIES;i++){
    if(enemies[i] == NULL){
      enemies[i] = new Enemy(x,y);
      return;
    }
  }
}
