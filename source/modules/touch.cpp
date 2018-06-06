#include "common/runtime.h"
#include "modules/touch.h"

//Löve2D Functions

//love.touch.getPosition
int Touch::GetPosition(lua_State * L)
{
    if (lua_islightuserdata(L, 1))
    {
        int * id = (int *)lua_topointer(L, 1);

        SDL_Finger * touch = SDL_GetTouchFinger(0, *id);

        lua_pushinteger(L, touch->x);
        lua_pushinteger(L, touch->y);

        return 2;
    }

    return 0;
}

//love.touch.getTouches
int Touch::GetTouches(lua_State * L)
{
    lua_newtable(L);

    uint touchCount = SDL_GetNumTouchFingers(0);
    for (u32 i = 0; i < touchCount; i++)
    {
        lua_pushnumber(L, i);
        lua_pushlightuserdata(L, &i);

        lua_settable(L, -3);
    }

    return 1;
}

int Touch::GetPressure(lua_State * L)
{
    if (lua_islightuserdata(L, 1))
    {
        int *id = (int *)lua_topointer(L, 1);

        SDL_Finger *touch = SDL_GetTouchFinger(0, *id);

        lua_pushinteger(L, touch->pressure);

        return 1;
    }

    lua_pushinteger(L, 0);

    return 1;
}

//End Löve2D Functions

int Touch::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "getPosition",    GetPosition    },
        { "getTouches",        GetTouches    },
        { "getPressure",    GetPressure    },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}