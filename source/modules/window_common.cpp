#include "common/runtime.h"
#include "modules/window_common.h"

//Löve2D Functions
int Window::SetMode(lua_State * L)
{
    //stubbed because this is useless

    return 0;
}

int Window::GetFullscreenModes(lua_State * L)
{
    lua_createtable(L, 1, 0);
    lua_pushnumber(L, 1);

    lua_createtable(L, 0, 2);

    //Inner table attributes
    lua_pushnumber(L, 1280);
    lua_setfield(L, -2, "width");
    
    lua_pushnumber(L, 720);
    lua_setfield(L, -2, "height");

    //End table attributes

    lua_settable(L, -3);

    return 1;
}

//End Löve2D Functions

int Window::Register(lua_State * L)
{
    Window::Initialize();
    
    luaL_Reg reg[] = 
    {
        { "setMode",            SetMode            },
        //{ "showMessageBox",     ShowMessageBox     },
        { "getFullscreenModes", GetFullscreenModes },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}