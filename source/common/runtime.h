#pragma once

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>

	#include <compat-5.2.h>
	#include <luaobj.h>
}

#include <3ds.h>
#include <citro3d.h>

#include <lodepng.h>

#include <ivorbiscodec.h>
#include <ivorbisfile.h>

#include <json.hpp>

#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <malloc.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

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

extern touchPosition touch;
extern bool touchDown;
extern int lastTouch[2];