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
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL2_rotozoom.h>
#include <SDL2_gfxPrimitives.h>
#include <png.h>

//C++ headers
#include <exception>
#include <string>
#include <vector>
#include <map>

//C headers
#include <errno.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <switch.h>

#include "common/types.h"
#include "common/util.h"
#include "common/console.h"
#include "common/exception.h"

using std::string;
using std::vector;
using std::pair;

extern FILE * logFile;

static inline void writeLog(const string & data)
{
	fwrite((char *)data.data(), 1, data.length(), logFile);

	fflush(logFile);
}

static inline void closeLog()
{
	if (logFile != NULL)
		fclose(logFile);
}