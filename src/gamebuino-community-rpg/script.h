#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <Arduino.h>
#include "settings.h"
class Script {
	int8_t vars[SCRIPT_NUM_VARS];
	bool condition();
	byte i,j;
	byte* ptr;
	byte* ptr2;
	void getVar();
	void readProg(byte* dst, byte size);
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

extern Script script;

#endif // _SCRIPT_H_
