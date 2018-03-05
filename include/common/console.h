/*
** console.h
** @brief	: Handles errors and logging
** @note	: Only displayed if conf.lua sets 't.console = true'
*/

#pragma once

namespace Console
{
	void Initialize(bool error = false);
	
	void ThrowError(const std::string & format, ...);
	void ThrowError(const std::string & errorMessage);
	int ThrowError(lua_State * L);
	
	const char * GetError();
	
	extern bool IsInitialized();
}