#pragma once

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <stdlib.h>
#include <string>
#include <malloc.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

extern bool LUA_ERROR;

#include "common/types.h"

#define luaL_dobuffer(L, b, n, s) \
	(luaL_loadbuffer(L, b, n, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

extern bool FUSED;
extern bool LOVE_QUIT;

extern bool AUDIO_ENABLED;
extern volatile bool updateAudioThread;

extern touchPosition touch;
extern bool touchDown;
extern int lastTouch[2];