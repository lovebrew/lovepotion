#include "common/runtime.h"
#include "modules/keyboard.h"
#include "objects/gamepad/gamepad.h"

#include "modules/event.h"

int lastTouch[2];
HID_Event event;

int Event::Poll(lua_State * L)
{
    Keyboard::Update(L);

    
    hidScanInput();

    //love.gamepadpressed
    u32 buttonDown = hidKeysDown();
    string downKey = DEVICE_INSTANCE->GetInput(buttonDown);

    touchPosition touch;
    hidTouchRead(&touch);

    if (downKey != "nil")
    {
        if (downKey != "touch")
        {
            love_getfield(L, "gamepadpressed");
            if (!lua_isnil(L, -1))
            {
                love_push_userdata(L, DEVICE_INSTANCE);
                lua_pushstring(L, downKey.c_str());

                lua_call(L, 2, 0);
            }
        }
        else
        {
            love_getfield(L, "touchpressed");
            if (!lua_isnil(L, -1))
            {
                lua_pushlightuserdata(L, (void *)1);
                lua_pushnumber(L, touch.px);
                lua_pushnumber(L, touch.py);
                lua_pushinteger(L, 0);
                lua_pushinteger(L, 0);
                lua_pushinteger(L, 1);

                lua_call(L, 6, 0);
            }
        }
    }

    u32 buttonHeld = hidKeysHeld();
    string heldKey = DEVICE_INSTANCE->GetInput(buttonHeld);

    if (heldKey != "nil" && heldKey == "touch")
    {
        lastTouch[0] = touch.px;
        lastTouch[1] = touch.py;

        love_getfield(L, "touchmoved");
        if (!lua_isnil(L, -1))
        {
            lua_pushlightuserdata(L, (void *)1);
            lua_pushnumber(L, touch.px);
            lua_pushnumber(L, touch.py);
            lua_pushinteger(L, 0);
            lua_pushinteger(L, 0);
            lua_pushinteger(L, 1);

            lua_call(L, 6, 0);
        }
    }

    //love.gamepadreleased
    u32 buttonUp = hidKeysUp();
    string upKey = DEVICE_INSTANCE->GetInput(buttonUp);

    if (upKey != "nil")
    {
        if (upKey != "touch")
        {
            love_getfield(L, "gamepadreleased");
            if (!lua_isnil(L, -1))
            {
                love_push_userdata(L, DEVICE_INSTANCE);
                lua_pushstring(L, upKey.c_str());

                lua_call(L, 2, 0);
            }
        }
        else
        {
            love_getfield(L, "touchreleased");
            if (!lua_isnil(L, -1))
            {
                lua_pushlightuserdata(L, (void *)1);
                lua_pushnumber(L, lastTouch[0]);
                lua_pushnumber(L, lastTouch[1]);
                lua_pushinteger(L, 0);
                lua_pushinteger(L, 0);
                lua_pushinteger(L, 1);

                lua_call(L, 6, 0);
            }
        }
    }

    for (uint i = 0; i < 11; i++)
    {
        love_getfield(L, "gamepadaxis");
        if (!lua_isnil(L, -1))
        {
            love_push_userdata(L, DEVICE_INSTANCE);
            lua_pushinteger(L, i + 1);
            lua_pushnumber(L, DEVICE_INSTANCE->GetAxis(i + 1));

            lua_call(L, 3, 0);
        }
    }

    return 0;
}

int Event::Quit(lua_State * L)
{
    LOVE_QUIT = true;

    return 0;
}

int Event::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "quit", Quit },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}