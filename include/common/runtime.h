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
#include <map>

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/util.h"
#include "common/console.h"

#include <switch.h>

using std::string;
using std::vector;
using std::pair;

//Error/Quit
extern bool ERROR;
extern bool LOVE_QUIT;