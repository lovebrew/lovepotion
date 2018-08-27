#include "common/runtime.h"
#include "modules/keyboard.h"

char buffer[255];
SwkbdState keyboard;
SwkbdButton button = SWKBD_BUTTON_NONE;

void Keyboard::Initialize()
{
    swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, -1);

    swkbdSetInitialText(&keyboard, buffer);
    swkbdSetHintText(&keyboard, "Input Text");

    swkbdSetFeatures(&keyboard, SWKBD_DEFAULT_QWERTY);
    swkbdSetValidation(&keyboard, SWKBD_NOTBLANK, 0, 0);

    swkbdSetButton(&keyboard, SWKBD_BUTTON_LEFT, "Cancel", false);
    swkbdSetButton(&keyboard, SWKBD_BUTTON_RIGHT, "Submit", true);
}

void Keyboard::Update(lua_State * L)
{
    if (button != SWKBD_BUTTON_RIGHT) //check for "Submit" button
        return;

    love_getfield(L, "textinput");
    lua_pushstring(L, buffer);
    lua_call(L, 1, 0);

    memset(buffer, 0, 255);

    button = SWKBD_BUTTON_NONE;
}

int Keyboard::SetTextInput(lua_State * L)
{
    button = swkbdInputText(&keyboard, buffer, sizeof(buffer));

    return 0;
}

int Keyboard::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "setTextInput", SetTextInput },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}