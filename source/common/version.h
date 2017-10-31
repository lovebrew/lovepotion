#pragma once

namespace love
{
	#define LOVE_VERSION "0.10.2"

	static const int VERSION_MAJOR = 0;
	static const int VERSION_MINOR = 10;
	static const int VERSION_REVISION = 2; 

	static const char * VERSION = LOVE_VERSION;
	static const char * CODENAME = "Super Toast";
}

extern bool IS_HOMEBREW;