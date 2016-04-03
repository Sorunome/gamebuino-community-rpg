
class Player {
  int8_t x=16,y=16;
  byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
  byte animation = 0;
  byte status = PLAYER_STATUS_IDLE;
  public:
    bool update(){
      byte flag;
      if(status == PLAYER_STATUS_IDLE){
        int8_t x_temp = -gb.buttons.repeat(BTN_LEFT, 1)+gb.buttons.repeat(BTN_RIGHT, 1);
        int8_t y_temp = -gb.buttons.repeat(BTN_UP, 1)+gb.buttons.repeat(BTN_DOWN, 1);
        if(x_temp || y_temp){
          direction = (1+x_temp)*(x_temp != 0);
          direction = (2+y_temp)*(y_temp != 0 || direction == 0);
          if(getWalkInfo(x+2,y+4,4,4,x_temp,0) <= FLAG_TILE_WALKABLE){
            focusCam();
            x += x_temp;
            if(x < 0){
              currentMap--;
              x = (TILEMAP_WIDTH*8) - 8;
              return false;
            }
            if(x > (TILEMAP_WIDTH*8) - 8){
              currentMap++;
              x = 0;
              return false;
            }
          }
          flag = getWalkInfo(x+2,y+4,4,4,0,y_temp);
          if(flag <= FLAG_TILE_WALKABLE){
            focusCam();
            y += y_temp; // below here else diagonal movements may screw up while switching stuff
            if(y < 0){
              currentMap -= DATFILE_TILEMAPS_WIDTH;
              y = (TILEMAP_HEIGHT*8) - 8;
              return false;
            }
            if(y > (TILEMAP_HEIGHT*8) - 8){
              currentMap += DATFILE_TILEMAPS_WIDTH;
              y = 0;
              return false;
            }
            animation = millis()/ANIMATION_FREQUENCY%2;
          }
          if(flag == FLAG_TILE_FALL && y_temp > 0){
            status = PLAYER_STATUS_FALL;
          }
        }
        return true;
      }
      if(status == PLAYER_STATUS_FALL){
        y++;
        if(y > (TILEMAP_HEIGHT*8) - 8){
          currentMap += DATFILE_TILEMAPS_WIDTH;
          y = 0;
          return false;
        }
        if(getWalkInfo(x+2,y,4,4,0,1) <= FLAG_TILE_WALKABLE && getWalkInfo(x+2,y+4,4,4,0,1) <= FLAG_TILE_WALKABLE){
          status = PLAYER_STATUS_IDLE;
        }
        focusCam();
        return true;
      }
    }
    void focusCam(){
      moveCam(x - (LCDWIDTH / 2), y - (LCDHEIGHT / 2));
    }
    void draw(){
      for(byte i = 0;i < 16;i++){
        tmpsprite[i] = pgm_read_byte(charset_player+(direction*24*2+animation*8*2)+i);
      }
      sprite_masked(tmpsprite, x, y);
    }
};
Player player;
