#pragma once

class Window
{
    public:
        Window() = delete;

        static int Register(lua_State * L);

    private:
        // Löve2D Functions

        static int SetMode(lua_State * L);

        static int GetFullscreenModes(lua_State * L);

        static int ShowMessageBox(lua_State * L);

        // End Löve2D Functions
};
