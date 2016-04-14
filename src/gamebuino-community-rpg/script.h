#define SCRIPT_NOP 0x00
#define SCRIPT_FADE_TO_WHITE 0x01
#define SCRIPT_FADE_FROM_WHITE 0x02
#define SCRIPT_SET_MAP 0x03
#define SCRIPT_TEXT 0x04
#define SCRIPT_ADD_ENEMY 0x05
#define SCRIPT_FOCUS_CAM 0x06
#define SCRIPT_UPDATE_SCREEN 0x07
#define SCRIPT_SET_VAR 0x08
#define SCRIPT_JUMP 0x09
#define SCRIPT_JUMP_IFNOT 0x0A
#define SCRIPT_LT 0x0B
#define SCRIPT_ADD 0x0C
#define SCRIPT_JUMP_IF 0x0D
#define SCRIPT_INC 0x0E
#define SCRIPT_DEC 0x0F
#define SCRIPT_CALL 0x10
#define SCRIPT_RET 0x11

#define SCRIPT_RETURN_FALSE 0xFE
#define SCRIPT_RETURN_TRUE 0xFF


#define SCRIPT_VAR_PLAYER_X 0xFF
#define SCRIPT_VAR_PLAYER_Y 0xFE
#define SCRIPT_VAR_MAP 0xFD
#define SCRIPT_VAR_WORLD 0xFC


/*
	you can use i and j almost freely
	BEWARE!!!!! If j is 0xFF (255) when calling readProg it'll re-fetch the buffer off of the sd card, it may slow stuff down
*/
void setScreenContrast_unsafe(byte contrast){
	gb.display.command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
	gb.display.command(PCD8544_SETVOP | contrast);
	gb.display.command(PCD8544_FUNCTIONSET);
}

void Script::getVar(void) {
	readProg(&i,1);
	cursor++;
	switch(i){
		case SCRIPT_VAR_PLAYER_X:
			ptr = (byte*)player.px;
			return;
		case SCRIPT_VAR_PLAYER_Y:
			ptr = (byte*)player.py;
			return;
		case SCRIPT_VAR_MAP:
			ptr = (byte*)&currentMap;
			return;
		case SCRIPT_VAR_WORLD:
			ptr = (byte*)&currentWorld;
			return;
		default:
			ptr = (byte*)&(vars[i]);
	}
}

void Script::getNum(byte* var){
	readProg(var,1);
	cursor++;
	if(*var == 0x80){
		readProg(var,1);
		if(*var == 0x80){
			cursor++; // inside here as getVar() will need to read the same byte
			*var = 0x80;
			return;
		}
		ptr2 = ptr;
		getVar();
		*var = (byte)*ptr;
		ptr = ptr2;
	}
}

void Script::readProg(byte* dst,byte size){
	if(j == 0xFF && cursor_loaded){
		datfile.read(screenbuffer,cursor_loaded,512);
		j = 0;
	}
	if(cursor_loaded == 0 || cursor < cursor_loaded || (cursor + size) > (cursor_loaded + 512)){
		datfile.read(screenbuffer,cursor,512);
		cursor_loaded = cursor;
	}
	memcpy(dst,screenbuffer + (cursor - cursor_loaded),size);
}
void Script::loadInTilemap(byte offset){
	i = 0;
	j = 0;

	datfile.read(screenbuffer,DATFILE_START_SCRIPT,42*6); // 42*6 as it fits in one byte and is devisible by six
	while(true){
		if(screenbuffer[i] == 0xFF){
			cursor = 0;
			return;
		}
		if(screenbuffer[i] == offset && screenbuffer[i+1] == currentMap){
			cursor = (uint16_t(screenbuffer[i+2])+(uint16_t(screenbuffer[i+3])<<8)+(uint32_t(screenbuffer[i+4])<<16)+(uint32_t(screenbuffer[i+5])<<24));
			break;
		}
		i += 6;
		if(i >= 42*6){
			i = 0;
			datfile.read(screenbuffer,DATFILE_START_SCRIPT + ((42*6)*(uint32_t)j),42*6);
			j++;
		}
	}
}

bool Script::run(void){
	if(!cursor){
		gb.display.clear();
		return true;
	}
	j = 0xFF;
	cursor_loaded = 0;
	//drawScreen();
	while(true){
		readProg(&i,1);
		cursor++;
		switch(i){
			case SCRIPT_FADE_TO_WHITE:
				i = gb.display.contrast + 1;
				do{
					setScreenContrast_unsafe(--i);
					delay(7);
				}while(i > 0);
				continue;
			case SCRIPT_FADE_FROM_WHITE:
				for(i=0;i <= gb.display.contrast;i++){
					setScreenContrast_unsafe(i);
					delay(7);
				}
				continue;
			case SCRIPT_SET_MAP:
				getNum(&currentMap);
				loadTilemap(currentMap);
				j = 0xFF;
				continue;
			case SCRIPT_TEXT:
				dispText();
				continue;
			case SCRIPT_ADD_ENEMY:
				getNum(&i);
				i = addEnemy(i);
				getNum((byte*)&(enemies[i]->x));
				getNum((byte*)&(enemies[i]->y));

				continue;
			case SCRIPT_FOCUS_CAM:
				player.focusCam();
				continue;
			case SCRIPT_UPDATE_SCREEN:
				drawScreen();
				gb.display.update();
				j = 0xFF;
				continue;
			case SCRIPT_SET_VAR:
				getVar();
				getNum((byte*)ptr);
				continue;
			case SCRIPT_JUMP:
				readProg((byte*)&cursor,1);
				continue;
			case SCRIPT_JUMP_IFNOT:
				if(condition()){
					cursor += 4;
					continue;
				}
				readProg((byte*)&cursor,1);
				continue;
			case SCRIPT_JUMP_IF:
				if(condition()){
					readProg((byte*)&cursor,1);
					continue;
				}
				cursor += 4;
				continue;
			case SCRIPT_ADD:
				getVar();
				getNum(&j);
				*ptr += j;
				continue;
			case SCRIPT_INC:
				getVar();
				(*ptr)++;
				continue;
			case SCRIPT_DEC:
				getVar();
				(*ptr)--;
				continue;
			case SCRIPT_CALL:
				cursor_call = cursor+4;
				readProg((byte*)&cursor,1);
				continue;
			case SCRIPT_RET:
				cursor = cursor_call;
				continue;

			case SCRIPT_RETURN_FALSE:
				return false;
			case SCRIPT_RETURN_TRUE:
				gb.display.clear();
				return true;
		}
	}
}

bool Script::condition(void){
	readProg(&i,1);
	cursor++;
	switch(i){
		case SCRIPT_LT:
			getNum((byte*)&i);
			getNum((byte*)&j);
			return i < j;
	}
}

void Script::drawTextBox(void){
	drawScreen();
	gb.display.setColor(BLACK);
	gb.display.drawFastHLine(4,31,76);
	gb.display.drawFastHLine(4,45,76);
	gb.display.drawFastVLine(3,32,13);
	gb.display.drawFastVLine(80,32,13);
	gb.display.setColor(WHITE);
	gb.display.fillRect(4,32,76,13);
	gb.display.cursorX = 5;
	gb.display.cursorY = 33;
	gb.display.update();
	gb.display.setColor(BLACK);
}

void Script::dispText(void){
	drawTextBox();
	j = 0;
	while(true){
		if(!j && !gb.buttons.pressed(BTN_A)){
			j = 1;
		}
		datfile.read(&i,cursor++,1); // no read prog as we absolutly need the screen buffer!
		switch(i){
			case 0xFF:
				while(gb.buttons.pressed(BTN_A)); // we need to release the a button at least
				while(!gb.buttons.pressed(BTN_A));
				while(gb.buttons.pressed(BTN_A));

				j = 0xFF; // we need to re-fetch the program buffer
				return;
			case 0xFE:
				gb.display.cursorX = 5;
				gb.display.cursorY += 6;
				continue;
			case 0xFD:
				while(gb.buttons.pressed(BTN_A)); // we need to release the a button at least
				while(!gb.buttons.pressed(BTN_A));
				while(gb.buttons.pressed(BTN_A));
				j = 0;
				drawTextBox();
				continue;
			default:
				gb.display.drawChar(gb.display.cursorX,gb.display.cursorY,(char)i,1);
				gb.display.cursorX += 4;
				gb.display.update();
				if(!j || (j && !gb.buttons.pressed(BTN_A))){
					delay(100);
				}
		}
	}
}
