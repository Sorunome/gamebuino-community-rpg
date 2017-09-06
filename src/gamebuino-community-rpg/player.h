#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <Arduino.h>
enum class Player_Status : uint8_t {
	idle = 0,
	fall_wait,
	fall,
	sword,
	sword_spin,
	sword_spin_do_wait,
	sword_spin_do,
};

class Player {
	byte direction = 3; // 0 = right, 1 = up, 2 = left, 3 = down
	byte animation = 0;
	byte counter = 0;
	Player_Status status = Player_Status::idle;
	int8_t calcSwordX(bool cond);
	int8_t calcSwordY(bool cond);
	
	public:
		int8_t update_idle(int8_t& x_temp, int8_t& y_temp);
		int8_t update_fall_wait(int8_t& x_temp, int8_t& y_temp);
		int8_t update_fall(int8_t& x_temp, int8_t& y_temp);
		int8_t update_sword(int8_t& x_temp, int8_t& y_temp);
		int8_t update_sword_spin(int8_t& x_temp, int8_t& y_temp);
		int8_t update_sword_spin_do_wait(int8_t& x_temp, int8_t& y_temp);
		int8_t update_sword_spin_do(int8_t& x_temp, int8_t& y_temp);
	
		int16_t x=16*0x100,y=16*0x100;
		int16_t vx = 0;
		int16_t vy = 0;
		uint8_t acceleration = 0xFF;
		bool update();
		void focusCam();
		void draw();
};

extern Player player;

#endif // _PLAYER_H_
