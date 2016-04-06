
class Player {
  byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
  byte animation = 0;
  byte status = PLAYER_STATUS_IDLE;
  public:
    int8_t x=16,y=16;
    bool update(){
      byte flag;
      if(status == PLAYER_STATUS_IDLE){
        int8_t x_temp = -gb.buttons.pressed(BTN_LEFT)+gb.buttons.pressed(BTN_RIGHT);
        int8_t y_temp = -gb.buttons.pressed(BTN_UP)+gb.buttons.pressed(BTN_DOWN);
        if(x_temp || y_temp){
          animation = millis()/ANIMATION_FREQUENCY%2;
          direction = (1+x_temp)*(x_temp != 0);
          direction = (2+y_temp)*(y_temp != 0 || direction == 0);
          flag = getWalkInfo(x+2,y+4,4,4,x_temp,0);
          if(flag == FLAG_TILE_WALKABLE){
            focusCam();
            x += x_temp;
          }
          if(flag == FLAG_TILE_SCREENEND){
            if(x_temp < 0){
              currentMap--;
              x = (TILEMAP_WIDTH*8) - 8 + 2;
            }else{
              currentMap++;
              x = -2;
            }
            return false;
          }
          if(flag == FLAG_TILE_SCRIPT){
            return script.run(((x+2) / 8) + (((y+4) / 8)*TILEMAP_WIDTH));
          }
          
          
          flag = getWalkInfo(x+2,y+4,4,4,0,y_temp);
          if(flag <= FLAG_TILE_WALKABLE){
            focusCam();
            y += y_temp; // below here else diagonal movements may screw up while switching stuff
          }
          if(flag == FLAG_TILE_SCREENEND){
            if(y_temp < 0){
              currentMap -= DATFILE_TILEMAPS_WIDTH;
              y = (TILEMAP_HEIGHT*8) - 8 + 2;
            }else{
              currentMap += DATFILE_TILEMAPS_WIDTH;
              y = -2;
            }
            return false;
          }
          if(flag == FLAG_TILE_FALL && y_temp > 0){
            status = PLAYER_STATUS_FALL;
          }
          if(flag == FLAG_TILE_SCRIPT){
            return script.run(((x+2) / 8) + (((y+4) / 8)*TILEMAP_WIDTH));
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
      memcpy_P(tmpsprite,charset_player+(direction*24*2+animation*8*2),16);
      sprite_masked(tmpsprite, x, y);
    }
};
Player player;
