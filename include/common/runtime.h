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
#include <curl/curl.h>
#include <png.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL2_rotozoom.h>

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

#include <switch.h>

#include "objects/object/object.h"

#include "common/types.h"
#include "common/variant.h"
#include "common/util.h"
#include "modules/love.h"
#include "common/error.h"

#include "socket/luasocket.h"
#include "socket/common.h"


using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::map;
using std::queue;

extern FILE * logFile;

#define LOG(fmt, ...) fprintf(logFile, "%s:%d:\n" fmt "\n", __PRETTY_FUNCTION__, __LINE__, ## __VA_ARGS__)