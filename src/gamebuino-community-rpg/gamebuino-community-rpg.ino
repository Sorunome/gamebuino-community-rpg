#include "defines.h"
#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include <GB_Fat.h>

#define ENABLE_SOUND 0

#define TILEMAP_WIDTH 12
#define TILEMAP_HEIGHT 8
#define TILE_WIDTH 8
#define TILE_HEIGHT 8
#define FLAG_TILE_SCREENEND 0
#define FLAG_TILE_WALKABLE 1
#define FLAG_TILE_WALL 2
#define FLAG_TILE_WATER 3
#define FLAG_TILE_FALL 4
#define FLAG_TILE_SCRIPT 5

#define PLAYER_STATUS_IDLE 0
#define PLAYER_STATUS_FALL 1

#define ANIMATION_FREQUENCY 500 // ms
#define SOUNDBUFFER_PAGE ((const char*)(231 * 128))
#define SOUNDBUFFER_OFFSET (231 * 128)
#define MAX_NUM_ENEMIES 10

#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_HEIGHT)

byte tileset[8*TILEMAPS_SPRITESPACE];
byte tmpsprite[16];
byte walksprites[TILEMAPS_SPRITESPACE];
byte currentMap = TILEMAPS_DEFAULTMAP;

const byte charset_player[] PROGMEM  = {
  0x87,0x78, 0x87,0x48, 0x01,0xfe, 0x01,0xfe, 0x81,0x7a, 0x01,0xf2, 0x6f,0x90, 0xc0,0x2f,
  0xff,0x00, 0x87,0x78, 0x80,0x4f, 0x00,0xff, 0x00,0xfd, 0x80,0x79, 0xf7,0x08, 0xfb,0x04,
  0xff,0x00, 0xff,0x00, 0x01,0xfe, 0x01,0xce, 0x81,0x7a, 0x01,0xf2, 0x5f,0xa0, 0xdf,0x20,
  0xc0,0x2f, 0x41,0xbe, 0x01,0xfe, 0x81,0x7e, 0x01,0xfe, 0x01,0xfe, 0x87,0x78, 0xff,0x00,
  0xc3,0x2c, 0x40,0xbf, 0x00,0xff, 0x80,0x7f, 0x00,0xff, 0x00,0xff, 0x87,0x78, 0xff,0x00,
  0xdf,0x20, 0x41,0xbe, 0x01,0xfe, 0x81,0x7e, 0x01,0xfe, 0x01,0xfe, 0x87,0x78, 0xff,0x00,
  0xc0,0x2f, 0x6f,0x90, 0x01,0xf2, 0x81,0x7a, 0x01,0xfe, 0x01,0xfe, 0x87,0x48, 0x87,0x78,
  0xfb,0x04, 0xf7,0x08, 0x80,0x79, 0x00,0xfd, 0x00,0xff, 0x80,0x4f, 0x87,0x78, 0xff,0x00,
  0xdf,0x20, 0x5f,0xa0, 0x01,0xf2, 0x81,0x7a, 0x01,0xce, 0x01,0xfe, 0xff,0x00, 0xff,0x00,
  0x87,0x78, 0x01,0xce, 0x01,0xfa, 0x81,0x72, 0x01,0xfa, 0x41,0xbe, 0xc0,0x2f, 0xff,0x00,
  0x87,0x78, 0x00,0xff, 0x00,0xfd, 0x80,0x79, 0x00,0xfd, 0x40,0xbf, 0xc3,0x2c, 0xff,0x00,
  0x87,0x78, 0x01,0xce, 0x01,0xfa, 0x81,0x72, 0x01,0xfa, 0x41,0xbe, 0xdf,0x20, 0xff,0x00};

const byte charset_enemy[] PROGMEM = {
  0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF
};


Gamebuino gb;

void loadSong(uint16_t num);
void loadTilemap(uint8_t num);
byte getWalkInfo(int8_t x,int8_t y,byte w,byte h,int8_t dx,int8_t dy);
void moveCam(int8_t x,int8_t y);
void drawScreen();


byte camX = 0;
byte camY = 0;
byte tilemap[TILEMAP_SIZE];
byte firstAnimatedSprite;
GB_File datfile;
GB_File soundfile;
bool runScript(byte offset);
#include "graphics.h"
#include "player.h"
#include "enemies.h"
#include "script.h"



void loadSong(uint16_t num){
  gb.sound.stopTrack(0);
  gb.sound.stopTrack(1);
#if ENABLE_SOUND
  byte *buf = gb.display.getBuffer();
  soundfile.read(buf, num*1024, 512);
  write_flash_page((const char*)SOUNDBUFFER_OFFSET, buf);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128), buf+128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*2), buf+128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*3), buf+128*3);
  soundfile.read(buf, num*1024+512, 512);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*4), buf);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*5), buf+128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*6), buf+128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*7), buf+128*3);
  
  gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET),0);
  gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET + 40),1);
#endif
}

void loadTilemap(uint8_t num){
  uint8_t* buf = gb.display.getBuffer();
  uint8_t i;
  for(i = 0;i < MAX_NUM_ENEMIES;i++){ // delete all the enemies
    delete enemies[i];
    enemies[i] = NULL;
  }
  firstAnimatedSprite = TILEMAPS_SPRITESPACE;
  datfile.read(buf,DATFILE_START_TILEMAPLUT,256);
  for(i = 0;i < 256;i++){
    if(buf[i] == num){
      break;
    }
  }
  if(num == TILEMAP_2){
    addEnemy(3,1);
  }
  
  datfile.read(buf,(i*DATFILE_TILEMAP_SIZE) + DATFILE_START_TILEMAP + DATFILE_TILEMAPS_HEADER_SIZE,DATFILE_TILEMAP_SIZE - DATFILE_TILEMAPS_HEADER_SIZE);
  uint16_t* have_sprite_buf = (uint16_t*)buf + DATFILE_TILEMAP_SIZE - DATFILE_TILEMAPS_HEADER_SIZE;

  memset(have_sprite_buf,0xFF,2*TILEMAPS_SPRITESPACE);
  uint16_t* tmptilemapbuf = (uint16_t*)buf;
  byte* tmpsprite = (uint8_t*)have_sprite_buf + (2*TILEMAPS_SPRITESPACE);
  for(i = 0;i < TILEMAP_SIZE;i ++){
    if(tmptilemapbuf[i]>=SPRITES_FIRST_ANIMATED){
      for(num = TILEMAPS_SPRITESPACE-2;num >= 0;num-=2){
        if(have_sprite_buf[num] == tmptilemapbuf[i]){ // we don't need it anymore!
          tilemap[i] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){
          have_sprite_buf[num] = tmptilemapbuf[i];
          datfile.read(tmpsprite,((tmptilemapbuf[i] - SPRITES_FIRST_ANIMATED)*17) + DATFILE_START_SPRITES + (SPRITES_FIRST_ANIMATED*9),17);
          walksprites[num] = tmpsprite[0];
          memcpy(num*8 + tileset,tmpsprite+1,16);
          tilemap[i] = num;
          firstAnimatedSprite = num;
          break;
        }
      }
    }else{
      for(num = 0;num < TILEMAPS_SPRITESPACE;num++){
        if(have_sprite_buf[num] == tmptilemapbuf[i]){ // we don't need it anymore!
          tilemap[i] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){
          have_sprite_buf[num] = tmptilemapbuf[i];
          datfile.read(tmpsprite,(tmptilemapbuf[i]*9) + DATFILE_START_SPRITES,9);
          walksprites[num] = tmpsprite[0];
          memcpy(num*8 + tileset,tmpsprite+1,8);
          tilemap[i] = num;
          break;
        }
      }
    }
  }
}

byte getTileAt(int8_t x,int8_t y){
  return tilemap[(x / 8) + ((y/8) * TILEMAP_WIDTH)];
}
byte getFlagAt(int8_t x,int8_t y){
  byte tmp = walksprites[getTileAt(x,y)];
  return 1 + (((tmp >> ((x%8 < 4) + 2*(y%8 < 4))) & 0x01)*((tmp >> 4)+1));
}
byte getWalkInfo(int8_t x,int8_t y,byte w,byte h,int8_t dx,int8_t dy){
  byte tmp;
  w--;
  h--;
  if(dx){
    x += dx;
    if(dx > 0){
      if(x+w >= TILEMAP_WIDTH*8){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x+w,y);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x+w,y+h);
      }
      return tmp;
    }else{
      if(x < 0){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x,y);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x,y+h);
      }
      return tmp;
    }
  }
  if(dy){
    y += dy;
    if(dy > 0){
      if(y+h >= TILEMAP_HEIGHT*8){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x,y+h);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x+w,y+h);
      }
      return tmp;
    }else{
      if(y < 0){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x,y);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x+w,y);
      }
      return tmp;
    }
  }
  return FLAG_TILE_WALKABLE;
}

void moveCam(int8_t x,int8_t y){
  if(x < 0){
    camX = 0;
  }else if(x > (TILEMAP_WIDTH*8) - LCDWIDTH){
    camX = (TILEMAP_WIDTH*8) - LCDWIDTH;
  }else{
    camX = x;
  }
  if(y < 0){
    camY = 0;
  }else if(y > (TILEMAP_HEIGHT*8) - LCDHEIGHT){
    camY = (TILEMAP_HEIGHT*8) - LCDHEIGHT;
  }else{
    camY = y;
  }
}

void setup(){
  // put your setup code here, to run once:
  GB_Fat sd;
  
  gb.begin();
  gb.setFrameRate(40);
  gb.display.clear();
  gb.display.println(F("loading card..."));
  gb.display.update();

  if(sd.init(gb.display.getBuffer(), SPISPEED_VERYHIGH) != NO_ERROR){
    gb.display.clear();
    gb.display.print(F("SD card not found."));
    gb.display.update();
    while(1);
  }
  gb.display.clear();
  gb.display.println(F("Searching for file card..."));
  gb.display.update();
  
  soundfile = sd.open("SOUND.DAT", gb.display.getBuffer());
  if(!soundfile.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open sound file."));
    gb.display.update();
    while(1);
  }
  datfile = sd.open("DATA.DAT", gb.display.getBuffer());
  if(!datfile.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open file."));
    gb.display.update();
    while(1);
  }
  
  loadTilemap(currentMap);
  gb.display.clear();
  gb.display.println(F("SD card found."));
  gb.display.update();

  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 0);
  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 1);
  
  loadSong(0);
  
  //gb.display.setContrast(gb.display.contrast);
}

void drawScreen(){
  gb.display.clear();
  drawTilemap();
  player.draw();
  for(byte i = 0;i < MAX_NUM_ENEMIES;i++){
    if(enemies[i]!=NULL){
      enemies[i]->update();
      enemies[i]->draw();
    }
  }
  gb.display.update();
}

void loop(){
  // put your main code here, to run repeatedly:
  if(gb.update()){
    if(player.update()){
      drawTilemap();
      player.draw();
      for(byte i = 0;i < MAX_NUM_ENEMIES;i++){
        if(enemies[i]!=NULL){
          enemies[i]->update();
          enemies[i]->draw();
        }
      }
    }else{
      loadTilemap(currentMap);
      player.focusCam();
    }
  }
}
