void sprite_and(byte data[], byte x, byte y){
  uint8_t* buf = (((y+8)&0xF8)>>1) * 21 + x + gb.display.getBuffer();
  asm volatile(
  "mov R20,%[y]\n\t"
  "ldi R17,7\n\t"
  "add R20,R17\n\t"
  "brmi and_End\n\t"
  "cpi %[y],48\n\t"
  "brpl and_End\n\t"
 
  "inc R20\n\t"
  "ldi R16,8\n\t"
  "andi R20,7\n\t"
  "cpi R20,0\n\t"
  "breq and_LoopAligned\n"
  "and_LoopStart:\n\t"
  
    "tst %[x]\n\t"
    "brmi and_LoopSkip\n\t"
    "cpi %[x],84\n\t"
    "brcc and_LoopSkip\n\t"

    "ld R17,Z\n\t"
    "eor R18,R18\n\t"
    "mov R19,R20\n\t"
    "clc\n\t"
    "and_LoopShift:\n\t" // carry is still reset from the cpi instruction or from the dec
      "rol R17\n\t"
      "rol R18\n\t"
      "dec R19\n\t"
      "brne and_LoopShift\n\t"

    "tst %[y]\n\t"
    "brmi and_LoopSkipPart\n\t"
   
    "ld R19,X\n\t"
    "and R19,R17\n\t" //
    "st X,R19\n\t"
  "and_LoopSkipPart:\n\t"

    "cpi %[y],40\n\t"
    "brpl and_LoopSkip\n\t"
   
    "ld R19,Y\n\t"
    "and R19,R18\n\t" //
    "st Y,R19\n\t"

  "and_LoopSkip:\n\t"
    "eor R18,R18\n\t"
    "ldi R19,1\n\t"
    "add R26,R19\n\t" // INC DOESN'T CHANGE CARRY!
    "adc R27,R18\n\t"
    "add R28,R19\n\t"
    "adc R29,R18\n\t"
    "add R30,R19\n\t"
    "adc R31,R18\n\t"
   
    "inc %[x]\n\t"
    "dec R16\n\t"
    "brne and_LoopStart\n\t"
  "rjmp and_End\n"
  "and_LoopAligned:\n\t"
    "tst %[x]\n\t"
    "brmi and_LoopAlignSkip\n\t"
 
    "cpi %[x],84\n\t"
    "brcc and_LoopAlignSkip\n\t"
   
    "ld R17,Z\n\t"
    "ld R18,X\n\t"
    "and R18,R17\n\t" //
    "st X,R18\n\t"
  "and_LoopAlignSkip:\n\t"
    "ldi R18,1\n\t"
    "add R26,R18\n\t"
    "adc R27,R20\n\t"
    "add R30,R18\n\t"
    "adc R31,R20\n\t"
    "inc %[x]\n\t"
    "dec R16\n\t"
    "brne and_LoopAligned\n"
  "and_End:\n\t"
  ::"x" (buf - 84),"y" (buf),"z" (data),[y] "r" (y),[x] "r" (x):"r16","r17","r18","r19","r20");
}

void sprite_xor(byte data[], byte x, byte y){
  uint8_t* buf = (((y+8)&0xF8)>>1) * 21 + x + gb.display.getBuffer();
  asm volatile(
  "mov R20,%[y]\n\t"
  "ldi R17,7\n\t"
  "add R20,R17\n\t"
  "brmi xor_End\n\t"
  "cpi %[y],48\n\t"
  "brpl xor_End\n\t"
 
  "inc R20\n\t"
  "ldi R16,8\n\t"
  "andi R20,7\n\t"
  "cpi R20,0\n\t"
  "breq xor_LoopAligned\n"
  "xor_LoopStart:\n\t"
  
    "tst %[x]\n\t"
    "brmi xor_LoopSkip\n\t"
    "cpi %[x],84\n\t"
    "brcc xor_LoopSkip\n\t"

    "ld R17,Z\n\t"
    "eor R18,R18\n\t"
    "mov R19,R20\n\t"
    "clc\n\t"
    "xor_LoopShift:\n\t" // carry is still reset from the cpi instruction or from the dec
      "rol R17\n\t"
      "rol R18\n\t"
      "dec R19\n\t"
      "brne xor_LoopShift\n\t"

    "tst %[y]\n\t"
    "brmi xor_LoopSkipPart\n\t"
   
    "ld R19,X\n\t"
    "eor R19,R17\n\t"
    "st X,R19\n\t"
  "xor_LoopSkipPart:\n\t"

    "cpi %[y],40\n\t"
    "brpl xor_LoopSkip\n\t"
   
    "ld R19,Y\n\t"
    "eor R19,R18\n\t"
    "st Y,R19\n\t"

  "xor_LoopSkip:\n\t"
    "eor R18,R18\n\t"
    "ldi R19,1\n\t"
    "add R26,R19\n\t" // INC DOESN'T CHANGE CARRY!
    "adc R27,R18\n\t"
    "add R28,R19\n\t"
    "adc R29,R18\n\t"
    "add R30,R19\n\t"
    "adc R31,R18\n\t"
   
    "inc %[x]\n\t"
    "dec R16\n\t"
    "brne xor_LoopStart\n\t"
  "rjmp xor_End\n"
  "xor_LoopAligned:\n\t"
    "tst %[x]\n\t"
    "brmi xor_LoopAlignSkip\n\t"
 
    "cpi %[x],84\n\t"
    "brcc xor_LoopAlignSkip\n\t"
   
    "ld R17,Z\n\t"
    "ld R18,X\n\t"
    "eor R18,R17\n\t"
    "st X,R18\n\t"
  "xor_LoopAlignSkip:\n\t"
    "ldi R18,1\n\t"
    "add R26,R18\n\t"
    "adc R27,R20\n\t"
    "add R30,R18\n\t"
    "adc R31,R20\n\t"
    "inc %[x]\n\t"
    "dec R16\n\t"
    "brne xor_LoopAligned\n"
  "xor_End:\n\t"
  ::"x" (buf - 84),"y" (buf),"z" (data),[y] "r" (y),[x] "r" (x):"r16","r17","r18","r19","r20");
}

void draw_map(const byte tilemap[], int camera_x, int camera_y){
  byte map_width = tilemap[0];
  byte map_height = tilemap[1];
  for(byte y = 0; y <= 6; y++){
    for(byte x = 0; x <= 11; x++){
      int tile_x = camera_x/TILE_WIDTH+x;
      int tile_y = camera_y/TILE_HEIGHT+y;
      if(tile_x >= 0 && tile_x < map_width && tile_y >= 0 && tile_y < map_height){
        byte tile_num = tilemap[2+tile_y*map_width+tile_x];
        tile_num += (tile_num >= TILES_ANIMATED_START && tile_num <= TILES_ANIMATED_END)*millis()/ANIMATION_FREQUENCY%2;
        sprite_xor(tileset_forest_pointer+tile_num*TILE_HEIGHT, x*TILE_WIDTH-camera_x%TILE_WIDTH, y*TILE_HEIGHT-camera_y%TILE_HEIGHT);
      }
    }
  }
}

void draw_player(int x, int y, byte direction, byte animation){
  sprite_and(maskset_player_pointer+(direction*24+animation*8), x, y);
  sprite_xor(charset_player_pointer+(direction*24+animation*8), x, y);
}
