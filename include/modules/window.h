#pragma once

namespace Window
{
	void Initialize();

	SDL_Renderer * GetRenderer();

	SDL_Surface * GetSurface();

	void Exit();
}