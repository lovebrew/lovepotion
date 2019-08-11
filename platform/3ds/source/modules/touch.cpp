#include "common/runtime.h"
#include "modules/touch.h"
#include "modules/event.h"

// Löve2D Functions

//love.touch.getPosition
int Touch::GetPosition(lua_State * L)
{
    if (!LoveEvent::IsTouchDown())
        return 0;

    touchPosition touch;
    hidTouchRead(&touch);

    lua_pushinteger(L, touch.px + 1);
    lua_pushinteger(L, touch.py + 1);

    return 2;
}

//love.touch.getTouches
int Touch::GetTouches(lua_State * L)
{
    lua_newtable(L);

    if (!LoveEvent::IsTouchDown())
        return 1;

    uint touch = 1;

    lua_pushnumber(L, 1);
    lua_pushlightuserdata(L, &touch);

    lua_settable(L, -3);

    return 1;
}

//love.touch.getPressure
int Touch::GetPressure(lua_State * L)
{
    if (!LoveEvent::IsTouchDown())
        lua_pushinteger(L, 0);
    else
        lua_pushinteger(L, 1);

    return 1;
}

int Touch::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "getPosition", GetPosition },
        { "getPressure", GetPressure },
        { "getTouches",  GetTouches  },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}
