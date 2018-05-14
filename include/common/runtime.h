#pragma once

extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>
#include <citro2d.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <json.hpp>
#include <memory>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <array>
#include <map>
#include <vector>

#include "console.h"

#define CONFIG_3D_SLIDERSTATE (*(volatile float *)0x1FF81080)

using std::string;
using std::vector;

using json = nlohmann::json;

// Forward declare lua_State.
extern lua_State * L;

//Error/Quit
extern bool ERROR;
extern bool LOVE_QUIT;

extern int lastTouch[2];
extern bool touchDown;