#pragma once

namespace Socket
{
	int Preload(lua_State * L);

	int Initialize(lua_State * L);

	void Close();
}