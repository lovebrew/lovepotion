#include "common/runtime.h"
#include "modules/window.h"

SDL_Window * WINDOW;
SDL_Renderer * RENDERER;
SDL_Surface * WINDOW_SURFACE;

// create a 800x600 window for demonstration.
// if SDL_WINDOW_FULLSCREEN flag is passed, it will be hardware scaled (stretched) to fit screen,
// will always be centered and aspect ratio maintained.
// maximum window dimension is currently limited to 1280x720
void Window::Initialize()
{
	WINDOW = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_FULLSCREEN);

	if (!WINDOW)
		SDL_Quit();

	RENDERER = SDL_CreateRenderer(WINDOW, 0, SDL_RENDERER_SOFTWARE);

	if (!RENDERER)
		SDL_Quit();

	WINDOW_SURFACE = SDL_GetWindowSurface(WINDOW);
}

SDL_Renderer * Window::GetRenderer()
{
	return RENDERER;
}

SDL_Surface * Window::GetSurface()
{
	return WINDOW_SURFACE;
}

void Window::Exit()
{
	SDL_DestroyRenderer(RENDERER);
	SDL_DestroyWindow(WINDOW);
}