#include "common/runtime.h"
#include "modules/keyboard.h"

int Keyboard::ShowTextInput(lua_State * L)
{
    SwkbdConfig keyboard;
    char text[0x80] = {0};
    Result status;

    if (!lua_istable(L, 1) && !lua_isnoneornil(L, 1))
        luaL_error(L, "table expected, got %s", lua_type(L, 1));
    else if (lua_istable(L, 1))
        lua_settop(L, 1);
    else if (lua_isnoneornil(L, 1))
        lua_newtable(L);

    /*
    {
        [“type”] = “basic”,
        [“header”] = “Enter Text”,
        [“subheader”] = “”,
        [“hint”] = “Some Text”,
        [“isPassword”] = false
    }
    */

    lua_getfield(L, 1, "type");
    lua_getfield(L, 1, "inputLength");
    lua_getfield(L, 1, "header");
    lua_getfield(L, 1, "subheader");
    lua_getfield(L, 1, "hint");
    lua_getfield(L, 1, "isPassword");

    string type = luaL_optstring(L, -6, "basic");
    LOVE_VALIDATE_KEYBOARD_TYPE(type.c_str());

    u32 maxLength = clamp(0x1, luaL_optnumber(L, -5, 0x14), 0x80);

    const char * header = luaL_optstring(L, -4, "Enter Text");
    const char * subHeader = luaL_optstring(L, -3, "");
    const char * hint = luaL_optstring(L, -2, "");

    int isPassword = lua_toboolean(L, -1);

    lua_pop(L, 2);

    status = swkbdCreate(&keyboard, 0);
    
    if (R_FAILED(status))
        return 0;

    swkbdConfigMakePresetDefault(&keyboard);
    
    if (type == "standard")
        swkbdConfigSetType(&keyboard, SwkbdType_Normal);
    else if (type == "numpad")
        swkbdConfigSetType(&keyboard, SwkbdType_NumPad);

    swkbdConfigSetDicFlag(&keyboard, 1);
    swkbdConfigSetPasswordFlag(&keyboard, isPassword);

    swkbdConfigSetOkButtonText(&keyboard, "OK");

    swkbdConfigSetHeaderText(&keyboard, header);
    swkbdConfigSetSubText(&keyboard, subHeader);

    swkbdConfigSetGuideText(&keyboard, hint);

    status = swkbdShow(&keyboard, text, maxLength);
    
    if (R_SUCCEEDED(status))
    {
        love_getfield(L, "textinput");
        
        if (!lua_isnil(L, -1))
        {
            lua_pushstring(L, text);
            lua_call(L, 1, 0);
        }
    }

    swkbdClose(&keyboard);

    return 0;
}

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
