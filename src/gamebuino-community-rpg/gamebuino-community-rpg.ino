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
#define FLAG_TILE_MANUAL_SCRIPT 6

#define PLAYER_STATUS_IDLE 0
#define PLAYER_STATUS_FALL 1

#define ANIMATION_FREQUENCY 500 // ms
#define SOUNDBUFFER_PAGE ((const char*)(231 * 128))
#define SOUNDBUFFER_OFFSET (231 * 128)
#define MAX_NUM_ENEMIES 10

#define TILEMAP_SIZE (TILEMAP_WIDTH*TILEMAP_HEIGHT)

byte tileset[8*TILEMAPS_SPRITESPACE]; // this holds the current tileset used for the current tilemap
byte tmpsprite[16]; // temporary sprite buffer used for displaying sprites from PROGMEM
byte walksprites[TILEMAPS_SPRITESPACE]; // this buffer holds the data what is walkable and what isn't
byte currentMap = TILEMAPS_DEFAULTMAP; // the id of the current tilemap
byte currentWorld = 0; // the id of the current world
uint32_t mapAddress = 0; // the address to the tilemap on the sd card

// here is the player sprite
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
byte* screenbuffer = gb.display.getBuffer(); // allows for easier access, we use that buffer a lot all throughout the place


// we need to pre-declare a few functions
void loadSong(uint16_t num);
void loadTilemap(uint8_t num);
byte getWalkInfo(int8_t x,int8_t y,byte w,byte h,int8_t dx,int8_t dy);
void moveCam(int8_t x,int8_t y);
void drawScreen();


byte camX = 0; // camera x position
byte camY = 0; // camera y position
byte tilemap[TILEMAP_SIZE]; // the buffer of the tilemap
byte firstAnimatedSprite; // the marker for when the tiles are animated
GB_File datfile; // this is the main file, holding a whole bunch of information (sprites, tilemaps, scripts etc.)
GB_File soundfile; // this file will hold the sound

// we need to know what the script class looks already as we need to pre-decalre the script variable.
class Script {
  int8_t vars[SCRIPT_NUM_VARS];
  bool condition();
  byte i,j;
  byte* ptr;
  byte* ptr2;
  void getVar();
  void readProg(byte* dst,byte size);
  void getNum(byte* var);
  void dispText();
  void drawTextBox();
  public:
    uint32_t cursor;
    uint32_t cursor_loaded;
    uint32_t cursor_call;
    void loadInTilemap(byte offset);
    bool run();
};
Script script;
// these files end with .h as the arduino ide does weired stuff with .cpp and they need to be inserted at the correct position

// this file is used for graphics routines, such as drawing sprites and drawing the tilemap
#include "graphics.h"

// this file is used for the player control
#include "player.h"

// this file is used for the enemy control
#include "enemies.h"

// this file is used for the scripting system
#include "script.h"


/*
  loadSong

  this will load a sound and play it in the background

  takes: number of the song, starting with zero
  returns: none
*/
void loadSong(uint16_t num){
  gb.sound.stopTrack(0); // first stop the currently playing stuff
  gb.sound.stopTrack(1);
#if ENABLE_SOUND
  // a soundbuffer is 1024 bytes while the screenbuffer is only 512 bytes, thus we preform two reads
  soundfile.read(screenbuffer, num*1024, 512);
  write_flash_page((const char*)SOUNDBUFFER_OFFSET, screenbuffer); // one flash sector is 128 bytes (4*128 = 512)
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128), screenbuffer+128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*2), screenbuffer+128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*3), screenbuffer+128*3);
  soundfile.read(screenbuffer, num*1024+512, 512);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*4), screenbuffer);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*5), screenbuffer+128);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*6), screenbuffer+128*2);
  write_flash_page((const char*)(SOUNDBUFFER_OFFSET+128*7), screenbuffer+128*3);
  
  gb.sound.playTrack(0); // now start the tracks again, no need to specify where the buffer is as this is hardcoded
  gb.sound.playTrack(1);
  //gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET),0);
  //gb.sound.playTrack((const uint16_t *)(SOUNDBUFFER_OFFSET + 40),1);
#endif
}

/*
  exitTilemap

  This gets called when exiting a tilemap to check if there are any scripts we need to execute

  takes: none
  returns: true if continue normally (load the next tilemap), false if it already loaded a tilemap
*/
bool exitTilemap(){
  if(!mapAddress){ // if there is no previous map just don't do anything
    return true;
  }
  uint8_t i;
  datfile.read(&i,mapAddress,1);
  if(i&1){ // we have an exit script!
    datfile.read((byte*)&(script.cursor),mapAddress + DATFILE_TILEMAP_SIZE + 4,4);
    return script.run();
  }
  return true;
}

/*
  loadTilemap

  This will load a tilemap, populate the tileset and the tilemap buffer
  TODO: scripts on loading which spawn enemies etc.

  takes: number of the tilemap
  returns: none
*/
void loadTilemap(uint8_t num){
  uint8_t i;
  for(i = 0;i < MAX_NUM_ENEMIES;i++){ // delete all the enemies
    delete enemies[i];
    enemies[i] = NULL;
  }
  firstAnimatedSprite = TILEMAPS_SPRITESPACE; // time to reset the animated counter

  datfile.read((byte*)&mapAddress,DATFILE_START_TILEMAPLUT + (currentWorld*4),4); // fetch the address of the LUT for the current world

  // now we search for the address of the tilemap
  datfile.read(screenbuffer,mapAddress,256);
  i = 0;
  while(true){
    if(screenbuffer[i] == num){ // we found the map, now store the address
      memcpy(&mapAddress,&screenbuffer[i+1],4);
      break;
    }
    if(i==255){ // we need to load more stuff into the buffer
      mapAddress += 255;
      i = 0;
      datfile.read(screenbuffer,mapAddress,256);
      continue;
    }
    i += 5; // 5 = 1 + 4; 4 - address, 1 - tilemap num
  }

  // get the expanded tilemap into the screebnuffer
  datfile.read(screenbuffer,mapAddress,DATFILE_TILEMAP_SIZE);

  // this buffer is used to keep track of which sprites were already loaded and which new ID they have, 0xFF means an empty slot
  uint16_t* have_sprite_buf = (uint16_t*)screenbuffer + DATFILE_TILEMAP_SIZE;
  memset(have_sprite_buf,0xFF,2*TILEMAPS_SPRITESPACE);

  // just give it a different name for easier understanding
  uint16_t* tmptilemapbuf = (uint16_t*)(screenbuffer+DATFILE_TILEMAPS_HEADER_SIZE);

  // we cannot use tmpsprite for this as we need a 17-byte buffer (1-byte for walking information), fortunatly the screen buffer is large enough
  byte* tmpsprite = (uint8_t*)have_sprite_buf + (2*TILEMAPS_SPRITESPACE);

  for(i = 0;i < TILEMAP_SIZE;i ++){

    if(tmptilemapbuf[i]>=SPRITES_FIRST_ANIMATED){
      // the tile we'r looking at is animated, so we search from the back of the buffer if we have it already and if not import it
      for(num = TILEMAPS_SPRITESPACE-2;num >= 0;num-=2){
        if(have_sprite_buf[num] == tmptilemapbuf[i]){ // we don't need it anymore!
          tilemap[i] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){ // apparently the sprite wasn't loaded yet, time to import it!
          have_sprite_buf[num] = tmptilemapbuf[i]; // add it to the sprites we have
          // read the data from the sd card
          datfile.read(tmpsprite,((tmptilemapbuf[i] - SPRITES_FIRST_ANIMATED)*17) + DATFILE_START_SPRITES + (SPRITES_FIRST_ANIMATED*9),17);
          walksprites[num] = tmpsprite[0]; // set the walking data
          memcpy(num*8 + tileset,tmpsprite+1,16); // set the sprite data
          tilemap[i] = num;
          firstAnimatedSprite = num; // no need to perform additional logic on this, as due to how the logic will work this will always be smaller than the previous one.
          break;
        }
      }
    }else{
      // the tile we'r looking at is static, so we search from the front of the buffer if we have it loaded already
      for(num = 0;num < TILEMAPS_SPRITESPACE;num++){
        if(have_sprite_buf[num] == tmptilemapbuf[i]){ // we don't need it anymore!
          tilemap[i] = num;
          break;
        }
        if(have_sprite_buf[num] == 0xffff){ // we need to load the sprite!
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
  if(screenbuffer[0]&0x02){ // we have the header byte in here!
    // we need to run a start script!

    // we are using a custom script object as this may be called within another script, resulting in the cursor etc. being off
    Script tempscript;
    datfile.read((byte*)&(tempscript.cursor),mapAddress + DATFILE_TILEMAP_SIZE,4);
    tempscript.run();
  }
}

/*
  getTileAt

  returns the tile in the tilemap at pixel x/y coordinates

  takes: x,y
  returns: sprite id
*/
byte getTileAt(int8_t x,int8_t y){
  return tilemap[(x / 8) + ((y/8) * TILEMAP_WIDTH)];
}
/*
  getFlagAt

  returns the walking flag at pixel x/y coordinates

  takes: x,y
  returns: walking flag
*/
byte getFlagAt(int8_t x,int8_t y){
  byte tmp = walksprites[getTileAt(x,y)];
  return 1 + (((tmp >> ((x%8 < 4) + 2*(y%8 < 4))) & 0x01)*((tmp >> 4)+1));
}
/*
  getWalkInfo

  fetches the info if you can walk into a certain thing and which flags are present there

  takes: x,y,width,height,difference in x,difference in y
  returns: walking flag
*/
byte getWalkInfo(int8_t x,int8_t y,byte w,byte h,int8_t dx,int8_t dy){
  byte tmp;
  w--; // this makes our lives easier, as we start counting at zero then
  h--;
  if(dx){ // if we are moving in the x direction
    x += dx;
    if(dx > 0){ // we are moving right
      if(x+w >= TILEMAP_WIDTH*8){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x+w,y);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x+w,y+h);
      }
      return tmp;
    }else{
      if(x < 0){ // we are moving left
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x,y);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x,y+h);
      }
      return tmp;
    }
  }
  if(dy){ // if we are moving in the y direction
    y += dy;
    if(dy > 0){ // we are moving down
      if(y+h >= TILEMAP_HEIGHT*8){
        return FLAG_TILE_SCREENEND;
      }
      tmp = getFlagAt(x,y+h);
      if(tmp <= FLAG_TILE_WALKABLE){
        return getFlagAt(x+w,y+h);
      }
      return tmp;
    }else{
      if(y < 0){ // we are moving up
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

/*
  moveCam

  moves the camera to certain coordinates, paying attention that the cam doesn't go off-bounds

  takes: x,y
  returns: none
*/
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
  //Serial.begin(19200);
  //while(!Serial);
  //Serial.println("Starting game...");
  GB_Fat sd;
  gb.begin();
  gb.setFrameRate(40); // it just looked smoother IMO

  // TODO: Different displaying stuff while loading the card
  gb.display.clear();
  gb.display.println(F("loading card..."));
  gb.display.update();

  if(sd.init(screenbuffer, SPISPEED_VERYHIGH) != NO_ERROR){
    gb.display.clear();
    gb.display.print(F("SD card not found."));
    gb.display.update();
    while(1);
  }
  gb.display.clear();
  gb.display.println(F("Searching for file card..."));
  gb.display.update();
  
  soundfile = sd.open("SOUND.DAT", screenbuffer);
  if(!soundfile.exists()){
    gb.display.clear();
    gb.display.print(F("Couldn't open sound file."));
    gb.display.update();
    while(1);
  }
  datfile = sd.open("DATA.DAT", screenbuffer);
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

  //gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 0);
  //gb.sound.changePatternSet((const uint16_t* const*)(SOUNDBUFFER_OFFSET+80), 1);
  
  loadSong(0);
  //gb.display.setContrast(gb.display.contrast);
}

/*
  drawScreen

  clears the screen and draws it (tilemap, player, enemies). Call gb.display.update() manually!

  takes: none
  returns: none
*/
void drawScreen(){
  gb.display.clear();
  drawTilemap();
  player.draw();
  for(byte i = 0;i < MAX_NUM_ENEMIES;i++){
    if(enemies[i]!=NULL){
      enemies[i]->draw();
    }
  }
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
      // if we are here the player left a tilemap, time to load a different one! (currentMap already got updated)
      if(exitTilemap()){
        loadTilemap(currentMap);
        player.focusCam();
      }
    }
  }
}
