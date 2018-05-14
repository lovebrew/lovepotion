/*
** audio.h
** @brief	: Audio NDSP component control
** @pre		: ndsp dumped; placed in sdmc:/3ds/
** @warn	: Must be initialized for output
*/

#pragma once

namespace Audio
{
	void Initialize();

	//Löve2D Functions

	int Play(lua_State * L);
	int Stop(lua_State * L);
	
	int SetVolume(lua_State * L);

	//End Löve2D Functions
	
	int GetOpenChannel();

	void Exit();
	
	int Register(lua_State * L);
}

extern std::vector<bool> audioChannels;