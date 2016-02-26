// This code is licensed under the MIT Open Source License.

// Copyright (c) 2015 Ruairidh Carmichael - ruairidhcarmichael@live.co.uk

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <shared.h>

#define JOYSTICK_COUNT 1

int joystickInputScan(lua_State *L) { // love.joystick.scan()

	hidScanInput();
	
}

int joystickGetCount(lua_State *L) { //love.joystick.getJoystickCount()
	
	// Assuming a joystick in love is technically
	// a gamepad with a set of controls
	// then return 1 since the 3DS is a gamepad itself.
	lua_pushinteger(L, JOYSTICK_COUNT);
	
	return 1;
	
}

int joystickGetList(lua_State *L) { //love.joystick.getJoystickList()

	// Assuming a joystick in love is technically
	// a gamepad with a set of controls
	// then return a madeup joystick   
	lua_newtable(L);
	for(int i = 1; i <= JOYSTICK_COUNT; i++ ) //<< In the event that there might be more joysticks
	{
		lua_pushnumber(L, i);
		joystickNew(L, i);
		lua_rawset(L, -3);
	}
	
	return 1;

}

int joystickNew(lua_State *L);

int initLoveJoystick(lua_State *L) {
	
	luaL_Reg reg[] = {
		{ "getJoystickCount",   joystickGetCount },
		{ "getJoysticks",       joystickGetList },
		//{ "loadGamepadMappings",  gamepadLoadMappings},
		//{ "setGamepadMapping",    gamepadSetMapping},
		{ 0, 0 }
	};
	
	luaL_newlib(L, reg);
	
	return 1;
	
}