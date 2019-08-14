#pragma once

/*
** display.h
** @brief   : handles the actual renderer and window stuff 
*/

class Display
{
    public:
        Display() = delete;

        static void Initialize();

        static void Exit();

        static SDL_Renderer * GetRenderer();

    private:
        static inline SDL_Window * WINDOW = nullptr;
        static inline SDL_Renderer * RENDERER = nullptr;
};
