#pragma once

//Error/Quit
extern bool ERROR;
extern bool LOVE_QUIT;

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.3.h>
    #include <luaobj.h>
}

//Libs
#include <citro2d.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <png.h>
#include <curl/curl.h>

//C++ headers
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

//C headers
#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <3ds.h>

//Object--the only one needed here
#include "objects/object/object.h"

#include "socket/common.h"
#include "socket/luasocket.h"

//Common stuff
// #include "common/console.h"
#include "common/error.h"
#include "common/types.h"
#include "common/util.h"
// #include "common/variant.h"

//Löve module - important!
#include "modules/love.h"

using std::string;
using std::vector;