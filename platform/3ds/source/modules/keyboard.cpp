#include "common/runtime.h"
#include "modules/keyboard.h"

int Keyboard::ShowTextInput(lua_State * L)
{
    SwkbdState keyboard;
    char buffer[255];

    if (!lua_istable(L, 1) && !lua_isnoneornil(L, 1))
        luaL_error(L, "table expected, got %s", lua_type(L, 1));
    else if (lua_istable(L, 1))
        lua_settop(L, 1);
    else if (lua_isnoneornil(L, 1))
        lua_newtable(L);

    /*
    {
        [“type”] = “basic”,
        [“hint”] = “Some Text”,
        [“isPassword”] = false
    }
    */

    lua_getfield(L, 1, "type");
    lua_getfield(L, 1, "hint");
    lua_getfield(L, 1, "isPassword");

    string type = luaL_optstring(L, -3, "basic");
    const char * hint = luaL_optstring(L, -2, "Enter Text");
    int isPassword = lua_toboolean(L, -1);

    swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, -1);

    swkbdSetInitialText(&keyboard, "");
    swkbdSetValidation(&keyboard, SWKBD_NOTBLANK, 0, 0);

    swkbdSetButton(&keyboard, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&keyboard, SWKBD_BUTTON_RIGHT, "Submit", true);

    if (type == "basic")
        swkbdSetFeatures(&keyboard, SWKBD_DEFAULT_QWERTY);
    else if (type == "numpad")
        swkbdSetFeatures(&keyboard, SWKBD_TYPE_NUMPAD);

    swkbdSetHintText(&keyboard, hint);
    swkbdSetPasswordMode(&keyboard, (SwkbdPasswordMode)isPassword); //0 for anything not being 'true', 1 otherwise

    memset(buffer, 0, sizeof(buffer));

    SwkbdButton button = swkbdInputText(&keyboard, buffer, sizeof(buffer));

    if (button != SWKBD_BUTTON_RIGHT) //check for "Submit" button
        return 0;

    love_getfield(L, "textinput");
    lua_pushstring(L, buffer);
    lua_call(L, 1, 0);

    return 0;
}
