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

#define CLASS_TYPE LUAOBJ_TYPE_JOYSTICK
#define CLASS_NAME "Joystick"

char gamepadMapping[32][32] = {
	"a", "b", "back", "start",
	"dpright", "dpleft", "dpup", "dpdown",
	"rightshoulder", "leftshoulder", "x", "y",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", "",
	"", "", "", ""
};

const char *joystickInit(love_joystick *self, int id) {
	
	self->id = id;
	return NULL;

}

int joystickNew(lua_State *L, int id) {
		
	love_joystick *self = luaobj_newudata(L, sizeof(*self));
	
	luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);
	
	const char *error = joystickInit(self, id);
	
	if (error) luaError(L, error);
	
	return 1;
	
}

int joystickGetAxes(lua_State *L) {
	
	love_joystick *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	
	int numAxesCheck = lua_gettop(L);
	for (int i = 2; i <= numAxesCheck; i++) {
		
		int axisId = luaL_checkinteger(L, i);
		
		if( axisId < 5 ) { // Circle Axes
			
			circlePosition circleData;
			hidCircleRead(&circleData);

			if( axisId == 1 ) lua_pushinteger(L, circleData.dx);
			if( axisId == 2 ) lua_pushinteger(L, circleData.dy);
			
			circlePosition cStickData;
			irrstCstickRead(&cStickData);

			if( axisId == 3 ) lua_pushinteger(L, cStickData.dx);
			if( axisId == 4 ) lua_pushinteger(L, cStickData.dy);

		} else if( axisId < 8 ) { // Gyro Axes
			
			HIDUSER_EnableGyroscope();
			angularRate gyroData;
			hidGyroRead(&gyroData);
			
			if( axisId == 5 ) lua_pushinteger(L, gyroData.x);
			if( axisId == 6 ) lua_pushinteger(L, gyroData.y);
			if( axisId == 7 ) lua_pushinteger(L, gyroData.z);
			
		} else if ( axisId < 11 ) { // Accelloremeter Axes
		
			HIDUSER_EnableAccelerometer();
			accelVector accelData;
			hidAccelRead(&accelData);

			if( axisId == 8 ) lua_pushinteger(L, accelData.x);
			if( axisId == 9 ) lua_pushinteger(L, accelData.y);
			if( axisId == 10 ) lua_pushinteger(L, accelData.z);
		
		} else {
			luaError(L, "AxisId out of bounds");
		}
		
	}
	
	return numAxesCheck-1;
	
}

int joystickGetAxis(lua_State *L) {
	
	love_joystick *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	int axisId = luaL_checkinteger(L, 2);
	
	
	if( axisId < 3 ) { // Circle Axes
		
		circlePosition circleData;
		hidCircleRead(&circleData);

		if( axisId == 1 ) lua_pushinteger(L, circleData.dx);
		if( axisId == 2 ) lua_pushinteger(L, circleData.dy);
		
	} else if( axisId < 6 ) { // Gyro Axes
		
		HIDUSER_EnableGyroscope();
		angularRate gyroData;
		hidGyroRead(&gyroData);
		
		if( axisId == 3 ) lua_pushinteger(L, gyroData.x);
		if( axisId == 4 ) lua_pushinteger(L, gyroData.y);
		if( axisId == 5 ) lua_pushinteger(L, gyroData.z);
		
	} else if ( axisId < 9 ) { // Accelloremeter Axes
	
		HIDUSER_EnableAccelerometer();
		accelVector accelData;
		hidAccelRead(&accelData);
		if( axisId == 6 ) lua_pushinteger(L, accelData.x);
		if( axisId == 7 ) lua_pushinteger(L, accelData.y);
		if( axisId == 8 ) lua_pushinteger(L, accelData.z);
	
	} else {
		luaError(L, "AxisId out of bounds");
	}
	
	return 1;
	
}

int joystickGetAxisCount(lua_State *L) { // joystick:getAxisCount()
	
	// Circle X, Circle Y, 
	// Gyro X, Gyro Y, Gyro Z,
	// Accel X, Accel Y, Accel Z
	lua_pushinteger(L, 8);
	return 1;   

}

int joystickGetButtonCount(lua_State *L) { //joystick:getButtonCount()
	
	// DPDOWN, DPUP, DPLEFT, DPRIGHT
	// SL, SR, A, B, X, Y, START, SELECT
	lua_pushinteger(L, 12);
	return 1;
	
}

int joystickGetGUID(lua_State *L) { //joystick:getGUID()
	
	luaError(L, "joystick:getGamepadMapping() is not implemented");
	return 0;
	
}

int joystickGetGamepadAxis(lua_State *L) { //joystick:getGamepadAxis()
	
	luaError(L, "joystick:getGamepadMapping() is not implemented");
	return 0;
	
}

int joystickGetGamepadMapping(lua_State *L) { //joystick:getGamepadMapping()
	
	luaError(L, "joystick:getGamepadMapping() is not implemented");
	return 0;
	
}

int joystickGetHat(lua_State *L) { //joystick:getHat()
	
	 luaError(L, "joystick:getHat() is not implemented");
	return 0; //M'lua
	
}

int joystickGetHatCount(lua_State *L) { //joystick:getHatCount()
	
	lua_pushinteger(L, 0); //No fedoras to give, M'lady.
	return 1;
	
}

int joystickGetID(lua_State *L) { // joystick:getID()
	
	love_joystick *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	lua_pushinteger(L, self->id);
	return 1;
	
}

int joystickGetName(lua_State *L) { // joystick:getName()
	
	lua_pushstring(L, "3DS Controls");
	return 1;
	
}

int joystickGetVibration(lua_State *L) { //joystick:getVibration()
	
	//Fake vibration, we already report 
	//that vibration is not supported anyway
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	return 2;
	
}

int joystickIsConnected(lua_State *L) { // joystick:isConnected()
	
	lua_pushboolean(L, true);
	return 1;
	
}

int joystickIsDown(lua_State *L) { // joystick:isDown()

	//Check first argument is Joystick type
	love_joystick *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	
	//Get input
	hidScanInput();
	u32 kHeld = hidKeysHeld();
	
	//Check arguments given
	int numBtnsCheck = lua_gettop(L);
	for (int i = 2; i <= numBtnsCheck; i++) {
		
		int btnId = luaL_checkinteger(L, i);
		const char *str = gamepadMapping[btnId-1]; //1-based
		
		bool result = false;
		
		for( int i = 0; i < 32; i++ ) {
			if(kHeld & BIT(i)) {
				if( strcmp( gamepadMapping[i], "touch" ) != 0 ) {
					if( strcmp(str, gamepadMapping[i]) == 0) {
						result = true;
					}
				}
			}
		}
		
		lua_pushboolean(L, result);
	}
	return numBtnsCheck-1;

}

int joystickIsGamepad(lua_State *L) { //joystick:isGamepad()
	
	lua_pushboolean(L, true);
	return 1;
		
}

int joystickIsGamepadDown(lua_State *L) { //joystick:isGamepadDown()
	
	//Check first argument is Joystick type
	love_joystick *self = luaobj_checkudata(L, 1, CLASS_TYPE);
	
	//Get input
	hidScanInput();
	u32 kHeld = hidKeysHeld();
	
	//Check arguments given
	int numBtnsCheck = lua_gettop(L);
	for (int i = 2; i <= numBtnsCheck; i++) {
		const char *str = luaL_checkstring(L, i);
		
		bool result = false;
		
		for( int i = 0; i < 32; i++ ) {
			if(kHeld & BIT(i)) {
				if( strcmp( gamepadMapping[i], "touch" ) != 0 ) {
					if( strcmp(str, gamepadMapping[i]) == 0) {
						result = true;
					}
				}
			}
		}
		
		lua_pushboolean(L, result);
	}
	return numBtnsCheck-1;

}

int joystickIsVibrationSupported(lua_State *L) {
	
	lua_pushboolean(L, false);
	return 1;
	
}

int joystickSetVibration(lua_State *L) {
	
	return 1;
	
}

int initJoystickClass(lua_State *L) {
	
	luaL_Reg reg[] = {  
		  { "getAxes",              joystickGetAxes },
		  { "getAxis",              joystickGetAxis },
		  { "getAxisCount",         joystickGetAxisCount },
		  { "getButtonCount",       joystickGetButtonCount },
		  { "getGUID",              joystickGetGUID },
		  { "getGamepadAxis",       joystickGetGamepadAxis },
		  { "getGamepadMapping",    joystickGetGamepadMapping },
		  { "getHat",               joystickGetHat },
		  { "getHatCount",          joystickGetHatCount },
		  { "getID",                joystickGetID },
		  { "getName",              joystickGetName },
		  { "getVibration",         joystickGetVibration },
		  { "isConnected",          joystickIsConnected },
		  { "isDown",               joystickIsDown },
		  { "isGamepad",            joystickIsGamepad },
		  { "isGamepadDown",        joystickIsGamepadDown },
		  { "isVibrationSupported", joystickIsVibrationSupported },
		  { "setVibration",         joystickSetVibration },
		  { 0, 0 }
	};
	
	luaobj_newclass(L, CLASS_NAME, NULL, joystickNew, reg);
	
	return 1;   
	
}