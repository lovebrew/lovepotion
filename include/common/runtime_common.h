#pragma once

extern "C"
{
    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>

    #include <compat-5.3.h>
    #include <luaobj.h>
}

//Libraries
#include <SDL.h>
#include <SDL_mixer.h>
#include <curl/curl.h>

//C++ headers
#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <list>

//C headers
#include <errno.h>
#include <malloc.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//System Specific libraries
#if defined (_3DS)
    #include <3ds.h>
#elif defined (__SWITCH__)
    #include <switch.h>
#endif

//Object--the only one needed here
#include "objects/object/object.h"

#include "socket/common.h"
#include "socket/luasocket.h"

//Common stuff
#include "common/logger.h"
#include "common/error.h"
#include "common/types.h"
#include "common/util.h"
#include "common/variables.h"
#include "common/variant.h"
#include "common/version.h"

//Löve module - important!
#include "modules/love.h"

using std::string;
using std::vector;
using std::map;
using std::queue;
using std::list;
using std::max;
using std::min;
