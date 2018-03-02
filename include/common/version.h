#pragma once

namespace Love
{
	#define LOVE_VERSION "0.11.0"

	static const int VERSION_MAJOR = 0;
	static const int VERSION_MINOR = 11;
	static const int VERSION_REVISION = 0; 

	static const std::string VERSION = LOVE_VERSION;
	static const std::string CODENAME = "UNKNOWN";
}

extern bool IS_HOMEBREW;