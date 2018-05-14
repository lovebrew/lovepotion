#pragma once

namespace Love
{
	#define LOVE_VERSION "0.10.2"

	static const int VERSION_MAJOR = 0;
	static const int VERSION_MINOR = 10;
	static const int VERSION_REVISION = 2; 

	static const std::string VERSION = LOVE_VERSION;
	static const std::string CODENAME = "Super Toast";
}

extern bool IS_HOMEBREW;