/*
** audio.h
** @brief   : Audio component control
** @warn    : Must be initialized for output
*/

#pragma once

class Audio
{
    public:
        static void Initialize();

        static int GetOpenChannel();

        static void Exit();

        static int Register(lua_State * L);

    private:
        //Löve2D Functions

        static int Play(lua_State * L);

        static int Stop(lua_State * L);
        
        static int Pause(lua_State * L);
        
        static int Resume(lua_State * L);
        
        static int SetVolume(lua_State * L);

        //End Löve2D Functions
};
