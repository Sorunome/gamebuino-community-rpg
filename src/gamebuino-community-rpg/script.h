#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_SET_MAP 0x03
#define SCRIPT_SET_PLAYER_X 0x04
#define SCRIPT_SET_PLAYER_Y 0x05
#define SCRIPT_FOCUS_CAM 0x06
#define SCRIPT_UPDATE_SCREEN 0x07

#define SCRIPT_RETURN_FALSE 0xFE
#define SCRIPT_RETURN_TRUE 0xFF
void setScreenContrast_unsafe(byte contrast){
  gb.display.command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
  gb.display.command(PCD8544_SETVOP | contrast);
  gb.display.command(PCD8544_FUNCTIONSET);
}
bool runScript(byte offset){
  byte* buf = gb.display.getBuffer();
  byte i = 0;
  byte j = 0;
  uint32_t cursor;
  datfile.read(buf,DATFILE_START_SCRIPT,42*6); // 42*6 as it fits in one byte and is devisible by six
  while(true){
    if(buf[i] == 0xFF){
      gb.display.clear();
      return true;
    }
    if(buf[i] == offset && buf[i+1] == currentMap){
      cursor = (uint16_t(buf[i+2])+(uint16_t(buf[i+3])<<8)+(uint32_t(buf[i+4])<<16)+(uint32_t(buf[i+5])<<24));
      break;
    }
    i += 6;
    if(i >= 42*6){
      i = 0;
      datfile.read(buf,DATFILE_START_SCRIPT + ((42*6)*(uint32_t)j),42*6);
    }
  }
  drawScreen();
  while(true){
    datfile.read(&i,cursor++,1);
    switch(i){
      case SCRIPT_FADE_TO_WHITE:
        j = gb.display.contrast + 1;
        do{
          setScreenContrast_unsafe(--j);
          delay(7);
        }while(j > 0);
        break;
      case SCRIPT_FADE_FROM_WHITE:
        for(j=0;j <= gb.display.contrast;j++){
          setScreenContrast_unsafe(j);
          delay(7);
        }
        break;
      case SCRIPT_SET_MAP:
        datfile.read(&currentMap,cursor++,1);
        loadTilemap(currentMap);
        break;
      case SCRIPT_SET_PLAYER_X:
        datfile.read((uint8_t*)&(player.x),cursor++,1);
        break;
      case SCRIPT_SET_PLAYER_Y:
        datfile.read((uint8_t*)&(player.y),cursor++,1);
        break;
      case SCRIPT_FOCUS_CAM:
        player.focusCam();
        break;
      case SCRIPT_UPDATE_SCREEN:
        drawScreen();
        break;
      case SCRIPT_RETURN_FALSE:
        return false;
      case SCRIPT_RETURN_TRUE:
        return true;
    }
  }
}
