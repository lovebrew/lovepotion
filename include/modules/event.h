#pragma once

namespace Event
{
	int Poll(lua_State * L);

	int Quit(lua_State * L);

	int Register(lua_State * L);
};