// Lua
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>
#include <citro3d.h>

#include <ivorbiscodec.h>
#include <ivorbisfile.h>

#include <stdlib.h>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>

#include "common/console.h"
#include "common/types.h"

extern std::string debug;
extern love::Console * console;
extern bool FUSED;