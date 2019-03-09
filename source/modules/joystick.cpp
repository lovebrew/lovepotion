#include "common/runtime.h"

#include "objects/gamepad/gamepad.h"
#include "objects/gamepad/wrap_gamepad.h"
#include "modules/joystick.h"

void Joystick::Initialize(lua_State * L)
{
    #if defined (__SWITCH__)
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    #endif

    gamepadNew(L, 0);
}

Gamepad * Joystick::GetJoystickFromID(uint id)
{
    if (id >= controllers.size())
        return controllers[id - 1];

    return controllers[id];
}

//Löve2D Functions

//Super hacky workaround for now..
//love.joystick.getJoysticks
int Joystick::GetJoysticks(lua_State * L)
{
    lua_newtable(L);

    for (uint i = 0; i < controllers.size(); i++)
    {
        lua_pushnumber(L, i + 1);
        love_push_userdata(L, controllers[i]);

        lua_settable(L, -3);
    }

    return 1;
}


//love.joystick.getJoystickCount
int Joystick::GetJoystickCount(lua_State * L)
{
    lua_pushnumber(L, controllers.size());

    return 1;
}

//End Löve2D Functions

int Joystick::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "getJoystickCount", GetJoystickCount },
        { "getJoysticks",     GetJoysticks     },
        { 0, 0 },
    };

    luaL_newlib(L, reg);
    
    return 1;
}