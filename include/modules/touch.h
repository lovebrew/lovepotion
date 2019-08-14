#pragma once

class Touch
{
    public:
        Touch() = delete;

        static int Register(lua_State * L);

    private:
        //Löve2D Functions

        static int GetPosition(lua_State * L);

        static int GetTouches(lua_State * L);

        static int GetPressure(lua_State * L);

        //End Löve2D Functions
};
