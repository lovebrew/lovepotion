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
#include <citro2d.h>
#include <png.h>

//C++ headers
#include <exception>
#include <map>
#include <queue>

//C headers
#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <3ds.h>

#include "common/console.h"
#include "modules/love.h"

using std::string;