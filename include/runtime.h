#pragma once

extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include "console.h"

#include <switch.h>

using std::string;
using std::vector;

// Forward declare lua_State.
extern lua_State * L;

//Error/Quit
extern bool ERROR;
extern bool LOVE_QUIT;