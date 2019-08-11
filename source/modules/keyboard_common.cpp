#include "common/runtime.h"
#include "modules/keyboard.h"

int Keyboard::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "showTextInput", ShowTextInput },
        { 0, 0 },
    };

    luaL_newlib(L, reg);
    
    return 1;
}
