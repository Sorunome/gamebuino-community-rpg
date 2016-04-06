#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_SET_MAP 0x03
#define SCRIPT_SET_PLAYER_X 0x04
#define SCRIPT_SET_PLAYER_Y 0x05
#define SCRIPT_FOCUS_CAM 0x06
#define SCRIPT_UPDATE_SCREEN 0x07
#define SCRIPT_SET_VAR 0x08
#define SCRIPT_JUMP 0x09
#define SCRIPT_JUMP_IFNOT 0x0A
#define SCRIPT_LT 0x0B
#define SCRIPT_ADD 0x0C
#define SCRIPT_JUMP_IF 0x0D
#define SCRIPT_INC 0x0E
#define SCRIPT_DEC 0x0F

#define SCRIPT_RETURN_FALSE 0xFE
#define SCRIPT_RETURN_TRUE 0xFF

/*
  you can use i and j almost freely
  BEWARE!!!!! If j is 0xFF (255) when calling readProg it'll re-fetch the buffer off of the sd card, it may slow stuff down
*/
void setScreenContrast_unsafe(byte contrast){
  gb.display.command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
  gb.display.command(PCD8544_SETVOP | contrast);
  gb.display.command(PCD8544_FUNCTIONSET);
}

void Script::getVar(byte* var){
  readProg(var,1);
  cursor++;
  if(*var == 0x80){
    readProg(var,1);
    cursor++;
    if(*var == 0xFF){
      *var = 0x80;
      return;
    }
    *var = (byte)vars[*var];
  }
}

void Script::readProg(byte* dst,byte size){
  if(j == 0xFF && cursor_loaded){
    datfile.read(screenbuffer,cursor_loaded,512);
    j = 0;
  }
  if(cursor_loaded == 0 || cursor < cursor_loaded || (cursor + size) > (cursor_loaded + 512)){
    datfile.read(screenbuffer,cursor,512);
    cursor_loaded = cursor;
  }
  memcpy(dst,screenbuffer + (cursor - cursor_loaded),size);
}

bool Script::run(byte offset){
  i = 0;
  j = 0;
  
  datfile.read(screenbuffer,DATFILE_START_SCRIPT,42*6); // 42*6 as it fits in one byte and is devisible by six
  while(true){
    if(screenbuffer[i] == 0xFF){
      gb.display.clear();
      return true;
    }
    if(screenbuffer[i] == offset && screenbuffer[i+1] == currentMap){
      cursor = (uint16_t(screenbuffer[i+2])+(uint16_t(screenbuffer[i+3])<<8)+(uint32_t(screenbuffer[i+4])<<16)+(uint32_t(screenbuffer[i+5])<<24));
      break;
    }
    i += 6;
    if(i >= 42*6){
      i = 0;
      datfile.read(screenbuffer,DATFILE_START_SCRIPT + ((42*6)*(uint32_t)j),42*6);
      j++;
    }
  }
  j = 0xFF;
  cursor_loaded = 0;
  drawScreen();
  while(true){
    readProg(&i,1);
    cursor++;
    switch(i){
      case SCRIPT_FADE_TO_WHITE:
        i = gb.display.contrast + 1;
        do{
          setScreenContrast_unsafe(--i);
          delay(7);
        }while(i > 0);
        break;
      case SCRIPT_FADE_FROM_WHITE:
        for(i=0;i <= gb.display.contrast;i++){
          setScreenContrast_unsafe(i);
          delay(7);
        }
        break;
      case SCRIPT_SET_MAP:
        getVar(&currentMap);
        loadTilemap(currentMap);
        j = 0xFF;
        break;
      case SCRIPT_SET_PLAYER_X:
        getVar((byte*)&(player.x));
        break;
      case SCRIPT_SET_PLAYER_Y:
        getVar((byte*)&(player.y));
        break;
      case SCRIPT_FOCUS_CAM:
        player.focusCam();
        break;
      case SCRIPT_UPDATE_SCREEN:
        drawScreen();
        j = 0xFF;
        break;
      case SCRIPT_SET_VAR:
        readProg(&i,1);
        cursor++;
        getVar((byte*)&vars[i]);
        break;
      case SCRIPT_JUMP:
        readProg((byte*)&cursor,1);
        break;
      case SCRIPT_JUMP_IFNOT:
        if(condition()){
          cursor += 4;
          continue;
        }
        readProg((byte*)&cursor,1);
        break;
      case SCRIPT_JUMP_IF:
        if(condition()){
          readProg((byte*)&cursor,1);
          continue;
        }
        cursor += 4;
        break;
      case SCRIPT_ADD:
        readProg(&i,1);
        cursor++;
        getVar(&j);
        vars[i] += j;
        break;
      case SCRIPT_INC:
        readProg(&i,1);
        cursor++;
        vars[i]++;
        break;
      case SCRIPT_DEC:
        readProg(&i,1);
        cursor++;
        vars[i]--;
        break;
      
      case SCRIPT_RETURN_FALSE:
        return false;
      case SCRIPT_RETURN_TRUE:
        return true;
    }
  }
}

bool Script::condition(){
  readProg(&i,1);
  cursor++;
  switch(i){
    case SCRIPT_LT:
      getVar((byte*)&i);
      getVar((byte*)&j);
      return i < j;
  }
}