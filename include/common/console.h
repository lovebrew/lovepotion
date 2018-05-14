/*
** console.h
** @brief	: Handles errors and logging
** @note	: Only displayed if conf.lua sets 't.console = true'
*/

#pragma once

namespace Console
{
	void Initialize(gfxScreen_t screen);
	
	void ThrowError(const std::string & format, ...);
	int ThrowError(lua_State * L);
	
	char * GetError();

	extern gfxScreen_t GetScreen();
	
	extern bool IsInitialized();
}