#include "common/runtime.h"
#include "modules/event.h"

#include "objects/gamepad/gamepad.h"
#include "modules/joystick.h"
#include "modules/timer.h"

SDL_Event event;

int LoveEvent::PollEvent(lua_State * L)
{
    //update gamepad rumble
    for (Gamepad * joycon : controllers)
        joycon->Update(Timer::GetDelta());

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_JOYAXISMOTION:
            {
                Gamepad * controller = Joystick::GetJoystickFromID(event.jaxis.which);

                love_getfield(L, "gamepadaxis");
                if (!lua_isnil(L, -1))
                {
                    love_push_userdata(L, controller);
                    lua_pushstring(L, GAMEPAD_AXES[event.jaxis.axis].c_str());

                    float value = (float)event.jaxis.value / JOYSTICK_MAX;
                    controller->ClampAxis(value);

                    lua_pushnumber(L, value);
                    lua_call(L, 3, 0);
                }
                break;
            }    
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            {    
                Gamepad * controller = Joystick::GetJoystickFromID(event.jbutton.which);

                love_getfield(L, (event.type == SDL_JOYBUTTONDOWN) ? "gamepadpressed" : "gamepadreleased");

                if (!lua_isnil(L, -1))
                {
                    love_push_userdata(L, controller);
                    lua_pushstring(L, KEYS[event.jbutton.button].c_str());

                    lua_call(L, 2, 0);
                }
                break;
            }
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
            {
                love_getfield(L, (event.type == SDL_FINGERDOWN) ? "touchpressed" : "touchreleased");

                if (!lua_isnil(L, -1))
                {
                    lua_pushlightuserdata(L, (void *)event.tfinger.touchId);
                    lua_pushnumber(L, event.tfinger.x * 1280);
                    lua_pushnumber(L, event.tfinger.y * 720);
                    lua_pushnumber(L, 0);
                    lua_pushnumber(L, 0);
                    lua_pushnumber(L, event.tfinger.pressure);

                    lua_call(L, 6, 0);
                }
                break;
            }
            case SDL_FINGERMOTION:
            {
                love_getfield(L, "touchmoved");

                if (!lua_isnil(L, -1))
                {
                    lua_pushlightuserdata(L, (void *)event.tfinger.touchId);
                    lua_pushnumber(L, event.tfinger.x * 1280);
                    lua_pushnumber(L, event.tfinger.y * 720);
                    lua_pushnumber(L, event.tfinger.dx * 1280);
                    lua_pushnumber(L, event.tfinger.dy * 720);
                    lua_pushnumber(L, event.tfinger.pressure);

                    lua_call(L, 6, 0);
                }
                break;
            }
            default:
                break;
        }
    }

    return 0;
}

//Löve2D Functions

//love.event.quit
int LoveEvent::Quit(lua_State * L)
{
    LOVE_QUIT = true;

    return 0;
}

//End Löve2D Functions

int LoveEvent::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "quit", Quit },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}