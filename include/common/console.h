/*
** console.h
** @brief	: Handles errors and logging
** @note	: Only displayed if conf.lua sets 't.console = true'
*/

#pragma once

namespace Console
{
	void Initialize(lua_State * L, bool error);
	
	extern bool IsInitialized();
}