#pragma once

namespace Window
{
	void Initialize();

	SDL_Renderer * GetRenderer();

	SDL_Surface * GetSurface();

	int SetMode(lua_State * L);

	int Register(lua_State * L);

	void Exit();
}