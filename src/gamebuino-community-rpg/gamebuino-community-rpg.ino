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

#define ANIMATION_FREQUENCY 500 // ms
#define SOUNDBUFFER_PAGE ((const char*)(231 * 128))
#define SOUNDBUFFER_OFFSET (231 * 128)

#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_HEIGHT)

byte tileset[8*32];
byte tmpsprite[16];
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


Gamebuino gb;

byte camX = 0;
byte camY = 0;
byte tilemap[TILEMAP_SIZE];
byte firstAnimatedSprite;
#include "graphics.h"
GB_Fat sd;
GB_File datfile;
GB_File soundfile;


void loadSong(uint16_t num){
  gb.sound.stopTrack(0);
  gb.sound.stopTrack(1);
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
}

void loadTilemap(uint8_t num){
  uint8_t* buf = gb.display.getBuffer();
  uint8_t i;
  firstAnimatedSprite = 32;
  datfile.read(buf,DATFILE_START_TILEMAPLUT,256);
  for(i = 0;i < 256;i++){
    if(buf[i] == num){
      break;
    }
  }
  datfile.read(buf,(i*DATFILE_TILEMAP_SIZE) + DATFILE_START_TILEMAP + DATFILE_TILEMAPS_HEADER_SIZE,DATFILE_TILEMAP_SIZE - DATFILE_TILEMAPS_HEADER_SIZE);
  uint16_t* have_sprite_buf = (uint16_t*)buf + DATFILE_TILEMAP_SIZE - DATFILE_TILEMAPS_HEADER_SIZE;

  memset(have_sprite_buf,0xFF,2*32);
  for(i = 0;i < (DATFILE_TILEMAP_SIZE - DATFILE_TILEMAPS_HEADER_SIZE);i += 2){
    uint16_t spriteid = uint16_t(buf[i]<<8) + buf[i+1];
    if(spriteid>=SPRITES_FIRST_ANIMATED){
      for(num = 30;num >= 0;num-=2){
        if(have_sprite_buf[num] == spriteid){ // we don't need it anymore!
          tilemap[i/2] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){
          have_sprite_buf[num] = spriteid;
          datfile.read(num*8 + tileset,((spriteid - SPRITES_FIRST_ANIMATED)*16) + DATFILE_START_SPRITES + (SPRITES_FIRST_ANIMATED*8),16);
          tilemap[i/2] = num;
          firstAnimatedSprite = num;
          break;
        }
      }
    }else{
      for(num = 0;num < 32;num++){
        if(have_sprite_buf[num] == spriteid){ // we don't need it anymore!
          tilemap[i/2] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){
          have_sprite_buf[num] = spriteid;
          datfile.read(num*8 + tileset,(spriteid*8) + DATFILE_START_SPRITES,8);
          tilemap[i/2] = num;
          Serial.println(spriteid);
          break;
        }
      }
    }
  }
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


class Player {
  int8_t x=0,y=0;
  byte direction = 1; // 0 = right, 1 = up, 2 = left, 3 = down
  byte animation = 0;
  public:
    bool update(){
      int8_t x_temp = -gb.buttons.repeat(BTN_LEFT, 1)+gb.buttons.repeat(BTN_RIGHT, 1);
      int8_t y_temp = -gb.buttons.repeat(BTN_UP, 1)+gb.buttons.repeat(BTN_DOWN, 1);
      if(x_temp || y_temp){
        direction = (1+x_temp)*(x_temp != 0);
        direction = (2+y_temp)*(y_temp != 0 || direction == 0);
        if(true){
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
          animation = !animation;
        }
      }
      return true;
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

void setup(){
  // put your setup code here, to run once:
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
  loadSong(0);
  loadTilemap(currentMap);
  gb.display.clear();
  gb.display.println(F("SD card found."));
  gb.display.update();

  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 0);
  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 1);
  
}

void loop(){
  // put your main code here, to run repeatedly:
  if(gb.update()){
    #if ENABLE_SOUND
    if(!gb.sound.trackIsPlaying[0]){
      gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET),0);
      gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET + 40),1);
    }
    #endif
    if(player.update()){
      drawTilemap();
      player.draw();
    }else{
      loadTilemap(currentMap);
      player.focusCam();
    }
  }
}
