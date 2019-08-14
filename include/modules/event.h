/*
** modules/event.h
** @brief    : Handles events.. like quitting.. and other things
*/

#pragma once

class LoveEvent
{
    public:
        LoveEvent() = delete;

        static int Pump(lua_State * L);

        static bool IsTouchDown();

        static int Register(lua_State * L);

    private:
        std::queue<std::map<std::string, std::vector<Variant>>> poll;

        static inline bool touchDown = false;

        //Löve2D Functions

        static int Quit(lua_State * L);

        static int Poll(lua_State * L);

        //End Löve2D Functions
};
