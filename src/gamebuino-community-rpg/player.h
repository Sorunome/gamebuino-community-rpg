
class Player {
  byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
  byte animation = 0;
  byte counter = 0;
  byte status = PLAYER_STATUS_IDLE;
  public:
    int8_t x=16,y=16;
    bool update(){
      int8_t x_temp = 0;
      int8_t y_temp = 0;
      switch(status){
        case PLAYER_STATUS_IDLE:
          if(gb.buttons.pressed(BTN_A)){
            // A is pressed! determine what to do
            switch(direction){
              case 0:
                x_temp = 1;
                break;
              case 1:
                y_temp = -1;
                break;
              case 2:
                x_temp = -1;
                break;
              case 3:
                y_temp = 1;
            }
            if(getWalkInfo(x+2,y+4,4,4,x_temp,y_temp)==FLAG_TILE_MANUAL_SCRIPT){
              // we need to run a script
              if(x_temp > 0){
                x_temp = 4;
              }
              if(y_temp > 0){
                y_temp = 4;
              }
              script.loadInTilemap(((x+2+x_temp) / 8) + (((y+4+y_temp) / 8)*TILEMAP_WIDTH));
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

            if(x_temp){
              switch(getWalkInfo(x+2,y+4,4,4,x_temp,0)){
                case FLAG_TILE_WALKABLE:
                  focusCam();
                  x += x_temp;
                  counter = 0;
                  break;
                case FLAG_TILE_SCREENEND:
                  // we walked off the screen! We need to switch tilemaps!
                  if(x_temp < 0){
                    currentMap--;
                    x = (TILEMAP_WIDTH*8) - 8 + 2;
                  }else{
                    currentMap++;
                    x = -2;
                  }
                  counter = 0;
                  return false; // no way we are just continuing the loaded tilemap, we need to load a new one!
                case FLAG_TILE_SCRIPT:
                  if(x_temp > 0){
                    x_temp = 4;
                  }
                  script.loadInTilemap(((x+2+x_temp) / 8) + (((y+4) / 8)*TILEMAP_WIDTH));
                  return script.run();
              }
            }
            
            if(y_temp){
              switch(getWalkInfo(x+2,y+4,4,4,0,y_temp)){
                case FLAG_TILE_WALKABLE:
                  focusCam();
                  y += y_temp; // below here else diagonal movements may screw up while switching stuff
                  counter = 0;
                  break;
                case FLAG_TILE_SCREENEND:
                  // we walked off the screen! We need to switch tilemaps!
                  if(y_temp < 0){
                    currentMap -= DATFILE_TILEMAPS_WIDTH;
                    y = (TILEMAP_HEIGHT*8) - 8 ;
                  }else{
                    currentMap += DATFILE_TILEMAPS_WIDTH;
                    y = -4;
                  }
                  counter = 0;
                  return false;
                case FLAG_TILE_FALL:
                  if(y_temp > 0 && counter++ > 10){
                    counter = 0;
                    status = PLAYER_STATUS_FALL;
                  }
                  break;
                case FLAG_TILE_SCRIPT:
                  if(y_temp > 0){
                    y_temp = 4;
                  }
                  script.loadInTilemap(((x+2) / 8) + (((y+4+y_temp) / 8)*TILEMAP_WIDTH));
                  return script.run();
              }
            }
          }
          break;
        case PLAYER_STATUS_FALL:
          y++; // we fall DOWN
          if(y > (TILEMAP_HEIGHT*8) - 8){
            // we need to load a new map!
            currentMap += DATFILE_TILEMAPS_WIDTH;
            y = 0;
            return false;
          }
          // check if we landed on ground
          if(getWalkInfo(x+2,y,4,4,0,1) <= FLAG_TILE_WALKABLE && getWalkInfo(x+2,y+4,4,4,0,1) <= FLAG_TILE_WALKABLE){
            status = PLAYER_STATUS_IDLE;
          }
          focusCam();
          //break; // no need as it's the last one in the case-block
      }
      return true;
    }
    void focusCam(){
      // this just focuses the cam onto the player
      moveCam(x - (LCDWIDTH / 2), y - (LCDHEIGHT / 2));
    }
    void draw(){
      memcpy_P(tmpsprite,charset_player+(direction*24*2+animation*8*2),16);
      sprite_masked(tmpsprite, x, y);
    }
};
Player player;
