#ifndef _ENEMIES_H_
#define _ENEMIES_H_

#include <Arduino.h>

class Enemy {
	byte type;
	byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
	byte state; // 0 = walking right, 1 = walking up, 2 = walking left, 3 = walking down, 4 = wait, 5 = idle
	byte counter;
	public:
		byte x,y;
		Enemy(byte t, byte cx, byte cy);
		void update();
		void draw();
};
extern Enemy *enemies[];



#endif // _ENEMIES_H_
