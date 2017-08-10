#pragma once

namespace love
{
	#define LOVE_VERSION "0.10.2"

	static const int VERSION_MAJOR = 0;
	static const int VERSION_MINOR = 10;
	static const int VERSION_REVISION = 2; 

	static const char * VERSION = LOVE_VERSION;
	static const char * CODENAME = "Super Toast";
} //namespace love

extern bool LUA_ERROR;
extern bool AUDIO_ENABLED;
extern bool LOVE_QUIT;