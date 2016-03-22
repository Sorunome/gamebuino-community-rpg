void sprite_xor(byte data[], byte xx, byte yy){
  int8_t x = xx - camX;
  int8_t y = yy - camY;
  
  uint8_t* buf = (((y+8)&0xF8)>>1) * 21 + x + gb.display.getBuffer();
  asm volatile(
  "mov R20,%[y]\n\t"
  "ldi R17,7\n\t"
  "add R20,R17\n\t"
  "brmi End\n\t"
  "cpi %[y],48\n\t"
  "brpl End\n\t"
 
  "inc R20\n\t"
  "ldi R16,8\n\t"
  "andi R20,7\n\t"
  "cpi R20,0\n\t"
  "breq LoopAligned\n"
  "LoopStart:\n\t"
  
    "tst %[x]\n\t"
    "brmi LoopSkip\n\t"
    "cpi %[x],84\n\t"
    "brcc LoopSkip\n\t"

    "ld R17,Z\n\t"
    "eor R18,R18\n\t"
    "mov R19,R20\n\t"
    "clc\n\t"
    "LoopShift:\n\t" // carry is still reset from the cpi instruction or from the dec
      "rol R17\n\t"
      "rol R18\n\t"
      "dec R19\n\t"
      "brne LoopShift\n\t"

    "tst %[y]\n\t"
    "brmi LoopSkipPart\n\t"
   
    "ld R19,X\n\t"
    "eor R19,R17\n\t"
    "st X,R19\n\t"
  "LoopSkipPart:\n\t"

    "cpi %[y],40\n\t"
    "brpl LoopSkip\n\t"
   
    "ld R19,Y\n\t"
    "eor R19,R18\n\t"
    "st Y,R19\n\t"

  "LoopSkip:\n\t"
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
    "brne LoopStart\n\t"
  "rjmp End\n"
  "LoopAligned:\n\t"
    "tst %[x]\n\t"
    "brmi LoopAlignSkip\n\t"
 
    "cpi %[x],84\n\t"
    "brcc LoopAlignSkip\n\t"
   
    "ld R17,Z\n\t"
    "ld R18,X\n\t"
    "eor R18,R17\n\t"
    "st X,R18\n\t"
  "LoopAlignSkip:\n\t"
    "ldi R18,1\n\t"
    "add R26,R18\n\t"
    "adc R27,R20\n\t"
    "add R30,R18\n\t"
    "adc R31,R20\n\t"
    "inc %[x]\n\t"
    "dec R16\n\t"
    "brne LoopAligned\n"
  "End:\n\t"
  ::"x" (buf - 84),"y" (buf),"z" (data),[y] "r" (y),[x] "r" (x):"r16","r17","r18","r19","r20");
}
void drawTilemap(){
  int8_t startDdx = (-camX) / 8;
  int8_t startDdy = (-camY) / 8;
  int8_t maxDdx = (LCDWIDTH + 8 - 1 + camX) / 8;
  int8_t maxDdy = (LCDHEIGHT + 8 - 1 + camY) / 8;
  if(TILEMAP_WIDTH < maxDdx){
      maxDdx = TILEMAP_WIDTH;
  }
  if(TILEMAP_HEIGHT < maxDdy){
      maxDdy = TILEMAP_HEIGHT;
  }
  if(startDdx < 0){
      startDdx = 0;
  }
  if(startDdy < 0){
      startDdy = 0;
  }
  for(byte ddy = startDdy;ddy < maxDdy;ddy++){
    for(byte ddx = startDdx;ddx < maxDdx;ddx++){
      sprite_xor(sprites + tilemap[ddy*TILEMAP_WIDTH + ddx]*8,ddx*8,ddy*8);
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

