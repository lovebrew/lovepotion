#include "../include/lua/lua.h"
#include "../include/lua/lualib.h"
#include "../include/lua/lauxlib.h"

#include "../shared.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <3ds.h>
#include <sf2d.h>

u32 kDown;
u32 kHeld;
u32 kUp;

char dsNames[32][32] = {
		"a", "b", "select", "start",
		"dright", "dleft", "dup", "ddown",
		"rbutton", "lbutton", "x", "y",
		"", "", "lzbutton", "rzbutton",
		"", "", "", "",
		"touch", "", "", "",
		"cstickright", "cstickleft", "cstickup", "cstickdown",
		"cpadright", "cpadleft", "cpadup", "cpaddown"
};

char keyNames[32][32] = { // TODO: Improve these.
		"a", "b", "KEY_SELECT", "esc",
		"right", "left", "up", "down",
		"KEY_R", "KEY_L", "x", "y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"touch", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
};

int keyboardScan(lua_State *L) { // love.keyboard.scan()

	hidScanInput();

	kDown = hidKeysDown();
	kHeld = hidKeysHeld();
	kUp = hidKeysUp();

	return 0;

}

int keyboardIsDown(lua_State *L) { // love.keyboard.isDown()

	const char *key = luaL_checkstring(L, 1);

	int boolval = 0; // 0 == false; 1 == true

	int i;
	for (i = 0; i < 32; i++) {
		if (kHeld & BIT(i)) {
			if (strcmp(keyNames[i], "touch") != 0) { // Touch events should probably not be returned in love.keyboard.
				if (strcmp(key, keyNames[i]) == 0 || strcmp(key, dsNames[i]) == 0) {
					boolval = 1;
				}
			}
		} 
	}

	lua_pushboolean(L, boolval);

	return 1;

}

int initLoveKeyboard(lua_State *L) {

	registerFunction("keyboard", "scan", keyboardScan);
	registerFunction("keyboard", "isDown", keyboardIsDown);

	return 1;

}