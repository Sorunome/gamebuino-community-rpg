#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>
#include <GB_Fat.h>
Gamebuino gb;
#define TILEMAP_WIDTH 12
#define TILEMAP_HEIGHT 8
#define SOUNDBUFFER_PAGE ((const char*)(231 * 128))
#define SOUNDBUFFER_OFFSET (231 * 128)
byte camX = 0;
byte camY = 0;
byte sprites[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

byte sprite_player[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};
byte tilemap[TILEMAP_WIDTH * TILEMAP_HEIGHT];

GB_Fat sd;
GB_File file;
GB_File soundfile;

void loadSong(uint16_t num){
  byte * buf = gb.display.getBuffer();
  soundfile.read(buf,num*1024,512);
  write_flash_page((const char*)SOUNDBUFFER_OFFSET,buf);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128),buf + 128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*2),buf + 128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*3),buf + 128*3);
  soundfile.read(buf,num*1024 + 512,512);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*4),buf);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*5),buf + 128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*6),buf + 128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET + 128*7),buf + 128*3);
}


#include "graphics.h"

class Player {
  byte x=8,y=8;
  public:
    void update(){
      if(gb.buttons.repeat(BTN_RIGHT,0)){
        x++;
      }
      if(gb.buttons.repeat(BTN_LEFT,0)){
        x--;
      }
      if(gb.buttons.repeat(BTN_UP,0)){
        y--;
      }
      if(gb.buttons.repeat(BTN_DOWN,0)){
        y++;
      }
      sprite_xor(sprite_player,x,y);
      moveCam(x - (LCDWIDTH / 2), y - (LCDHEIGHT / 2));
    }
};

void setup() {
  // put your setup code here, to run once:
  gb.begin();
  gb.setFrameRate(40);
  gb.display.clear();
  gb.display.println(F("loading card..."));
  gb.display.update();

  if(sd.init(gb.display.getBuffer(),SPISPEED_VERYHIGH)!=NO_ERROR){
    gb.display.clear();
    gb.display.print(F("SD card not found."));
    gb.display.update();
    while(1);
  }
  gb.display.clear();
  gb.display.println(F("seaching for file card..."));
  gb.display.update();
  
  soundfile = sd.open("SOUND.DAT",gb.display.getBuffer());
  if(!soundfile.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open sound file."));
    gb.display.update();
    while(1);
  }
  file = sd.open("DATA.DAT",gb.display.getBuffer());
  if(!file.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open file."));
    gb.display.update();
    while(1);
  }
  loadSong(0);
  gb.display.clear();
  gb.display.println(F("card found"));
  gb.display.update();
  
  file.read(tilemap,0,96);
  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET + 80),0);
  gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET + 80),1);
  /*
  byte buffer[11];
  buffer[10] = '\0';
  file = sd.open("TEST.TXT",gb.display.getBuffer());
  if(!file.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open file."));
    gb.display.update();
    while(1);
  }
  file.read(buffer,32768 - 5,10);
  gb.display.clear();
  gb.display.println(reinterpret_cast<const char*>(buffer));
  gb.display.update();
  while(1);*/
}
Player player;
void loop() {
  // put your main code here, to run repeatedly:
  if(gb.update()){
    if(!gb.sound.trackIsPlaying[0]){
      gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET),0);
      gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET + 40),1);
    }
    drawTilemap();
    player.update();
  }
}
