/*
** modules/timer.h
** @brief : Used for time keeping
*/

#pragma once

class Timer
{
    public:
        Timer() = delete;

        static float GetDelta();
    
        static void Tick();

        static int Register(lua_State * L);

    private:
        static inline u32 lastTime = 0;
        static inline u32 currentTime = 0;
        static inline float deltatime = 0.0;
        
        static inline u32 lastCountTime = 0;
        static inline int totalFrames = 0;
        static inline int frames = 0;

        //Löve2D Functions
        
        static int GetFPS(lua_State * L);
        
        static int GetTime(lua_State * L);
    
        static int GetDelta(lua_State * L);
    
        static int Sleep(lua_State * L);
    
        static int Step(lua_State * L);
        
        //End Löve2D Functions
};
