/*
** console.h
** @brief	: Handles logs
** @note	: Only displayed if conf.lua sets 't.console = true'
*/

#pragma once

namespace Console
{
	void Initialize();
	
	extern bool IsInitialized();
}