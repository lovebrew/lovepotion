#pragma once

//Error/Quit
extern bool ERROR;
extern bool LOVE_QUIT;

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

//Libs
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

//C++ headers
#include <exception>
#include <string>
#include <vector>
#include <map>

//C headers
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "common/types.h"
#include "common/util.h"
#include "common/console.h"
#include "common/exception.h"

#include <switch.h>

#include <lodepng.h>

using std::string;
using std::vector;
using std::pair;