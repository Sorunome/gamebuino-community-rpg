#include "graphics.h"
#include "globals.h"
#include <Gamebuino.h>

/*
	sprite_masked

	this draws a masked sprite to the spritebuffer.
	data like <mask-byte><sprite-byte><mask-byte><sprite-byte> etc. Mask is OR'd, the shape of the sprite, the sprite is the actual sprite, will be AND'd

	takes: data (in ram),x,y
	returns: none
*/
void sprite_masked(byte data[], byte xx, byte yy) {
	int8_t x = xx - camX;
	int8_t y = yy - camY;

	uint8_t* buf = (((y+8)&0xF8)>>1) * 21 + x + screenbuffer;
	asm volatile(
	"mov R20,%[y]\n\t"
	"ldi R17,7\n\t"
	"add R20,R17\n\t"
	"brmi sprite_masked_End\n\t"
	"cpi %[y],48\n\t"
	"brpl sprite_masked_End\n\t"

	"inc R20\n\t"
	"ldi R16,8\n\t"
	"andi R20,7\n\t"
	"cpi R20,0\n\t"
	"breq sprite_masked_LoopAligned\n"
	"sprite_masked_LoopStart:\n\t"

		"ld R21,Z+\n\t"

		"tst %[x]\n\t"
		"brmi sprite_masked_LoopSkip\n\t"
		"cpi %[x],84\n\t"
		"brcc sprite_masked_LoopSkip\n\t"

		"ld R17,Z\n\t"
		"clr R15\n\t"
		"ldi R18,0xFF\n\t"
		"mov R19,R20\n\t"
		"clc\n\t"
		"sprite_masked_LoopShift:\n\t" // carry is still reset from the cpi instruction or from the dec
			"lsl R17\n\t"
			"rol R15\n\t"

			"sec\n\t"
			"rol R21\n\t"
			"rol R18\n\t"
			"dec R19\n\t"
			"brne sprite_masked_LoopShift\n\t"

		"tst %[y]\n\t"
		"brmi sprite_masked_LoopSkipPart\n\t"

		"ld R19,X\n\t"
		"and R19,R21\n\t"
		"eor R19,R17\n\t"
		"st X,R19\n\t"
	"sprite_masked_LoopSkipPart:\n\t"

		"cpi %[y],40\n\t"
		"brpl sprite_masked_LoopSkip\n\t"

		"ld R19,Y\n\t"
		"and R19,R18\n\t"
		"eor R19,R15\n\t"
		"st Y,R19\n\t"

	"sprite_masked_LoopSkip:\n\t"

		"ld R19,X+\n\t" // less clock cycles to just load
		"ld R19,Y+\n\t"
		"ld R19,Z+\n\t"

		"inc %[x]\n\t"

		"dec R16\n\t"
		"brne sprite_masked_LoopStart\n\t"
	"rjmp sprite_masked_End\n"
	"sprite_masked_LoopAligned:\n\t"
		"ld R21,Z+\n\t"

		"tst %[x]\n\t"
		"brmi sprite_masked_LoopAlignSkip\n\t"

		"cpi %[x],84\n\t"
		"brcc sprite_masked_LoopAlignSkip\n\t"

		"ld R17,Z\n\t"
		"ld R18,X\n\t"
		"and R18,R21\n\t"
		"eor R18,R17\n\t"
		"st X,R18\n\t"
	"sprite_masked_LoopAlignSkip:\n\t"
		"ld R18,Z+\n\t" // less clock cycles to just load
		"ld R18,X+\n\t"

		"inc %[x]\n\t"
		"dec R16\n\t"
		"brne sprite_masked_LoopAligned\n"
	"sprite_masked_End:\n\t"
	::"x" (buf - 84),"y" (buf),"z" (data),[y] "d" (y),[x] "d" (x):"r16","r17","r18","r19","r20","r21","r15");
}

/*
	sprite_xor

	draws a sprite using XOR to the graph buffer

	takes: data (in ram),x,y
	returns: none
*/
void sprite_xor(byte data[], byte xx, byte yy) {
	int8_t x = xx - camX;
	int8_t y = yy - camY;

	uint8_t* buf = (((y+8)&0xF8)>>1) * 21 + x + screenbuffer;
	asm volatile(
	"mov R20,%[y]\n\t"
	"ldi R17,7\n\t"
	"add R20,R17\n\t"
	"brmi sprite_xor_End\n\t"
	"cpi %[y],48\n\t"
	"brpl sprite_xor_End\n\t"

	"inc R20\n\t"
	"ldi R16,8\n\t"
	"andi R20,7\n\t"
	"cpi R20,0\n\t"
	"breq sprite_xor_LoopAligned\n"
	"sprite_xor_LoopStart:\n\t"

		"tst %[x]\n\t"
		"brmi sprite_xor_LoopSkip\n\t"
		"cpi %[x],84\n\t"
		"brcc sprite_xor_LoopSkip\n\t"

		"ld R17,Z\n\t"
		"eor R18,R18\n\t"
		"mov R19,R20\n\t"

		"sprite_xor_LoopShift:\n\t"
			"lsl R17\n\t"
			"rol R18\n\t"
			"dec R19\n\t"
			"brne sprite_xor_LoopShift\n\t"

		"tst %[y]\n\t"
		"brmi sprite_xor_LoopSkipPart\n\t"

		"ld R19,X\n\t"
		"eor R19,R17\n\t"
		"st X,R19\n\t"
	"sprite_xor_LoopSkipPart:\n\t"

		"cpi %[y],40\n\t"
		"brpl sprite_xor_LoopSkip\n\t"

		"ld R19,Y\n\t"
		"eor R19,R18\n\t"
		"st Y,R19\n\t"

	"sprite_xor_LoopSkip:\n\t"

		"ld R19,X+\n\t" // less clock cycles to just load
		"ld R19,Y+\n\t"
		"ld R19,Z+\n\t"

		"inc %[x]\n\t"
		"dec R16\n\t"
		"brne sprite_xor_LoopStart\n\t"
	"rjmp sprite_xor_End\n"
	"sprite_xor_LoopAligned:\n\t"
		"tst %[x]\n\t"
		"brmi sprite_xor_LoopAlignSkip\n\t"

		"cpi %[x],84\n\t"
		"brcc sprite_xor_LoopAlignSkip\n\t"

		"ld R17,Z\n\t"
		"ld R18,X\n\t"
		"eor R18,R17\n\t"
		"st X,R18\n\t"
	"sprite_xor_LoopAlignSkip:\n\t"
		"ld R18,X+\n\t" // less clock cycles to just load
		"ld R18,Z+\n\t"

		"inc %[x]\n\t"
		"dec R16\n\t"
		"brne sprite_xor_LoopAligned\n"
	"sprite_xor_End:\n\t"
	::"x" (buf - 84),"y" (buf),"z" (data),[y] "d" (y),[x] "d" (x):"r16","r17","r18","r19","r20");
}

/*
	drawTilemap

	draws the tilemap to the graphbuffer, it assumes that the graphbuffer is clear

	takes: none
	returns: none
*/
void drawTilemap() {
	// OK, sorry, I do not remember how this is working, I just copypasted it off of my (sorunome) gb.display.tilemap thingy
	int8_t startDdx = (-camX) / 8;
	int8_t startDdy = (-camY) / 8;
	int8_t maxDdx = (LCDWIDTH + 8 - 1 + camX) / 8;
	int8_t maxDdy = (LCDHEIGHT + 8 - 1 + camY) / 8;
	if (TILEMAP_WIDTH < maxDdx) {
		maxDdx = TILEMAP_WIDTH;
	}
	if (TILEMAP_HEIGHT < maxDdy) {
		maxDdy = TILEMAP_HEIGHT;
	}
	if (startDdx < 0) {
		startDdx = 0;
	}
	if (startDdy < 0) {
		startDdy = 0;
	}
	for (byte ddy = startDdy; ddy < maxDdy; ddy++) {
		for (byte ddx = startDdx; ddx < maxDdx; ddx++) {
			byte tile_num = tilemap[ddy*TILEMAP_WIDTH + ddx];
			tile_num += (tile_num >= firstAnimatedSprite)*millis()/ANIMATION_FREQUENCY%2;
			sprite_xor(tileset+tile_num*TILE_HEIGHT, ddx*8, ddy*8);
		}
	}
}
