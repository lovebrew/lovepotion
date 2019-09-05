#include "common/runtime.h"
#include "modules/event.h"

#include "objects/gamepad/gamepad.h"
#include "modules/joystick.h"
#include "modules/timer.h"

SDL_Event event;
queue<map<string, vector<Variant>>> poll_queue;

int LoveEvent::Pump(lua_State * L)
{
    //update gamepad rumble
    std::vector<Gamepad *> controllers = Joystick::GetJoysticks();
    for (Gamepad * joycon : controllers)
        joycon->Update(Timer::GetDelta());

    while (SDL_PollEvent(&event))
    {
        map<string, vector<Variant>> args;
        
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

                    args["gamepadaxis"] = {Variant(GAMEPAD_AXES[event.jaxis.axis]), Variant(value)};
                    poll_queue.push(args);
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
            case SDL_JOYDEVICEADDED:
            case SDL_CONTROLLERDEVICEADDED:
                break;
            case SDL_JOYDEVICEREMOVED:
            case SDL_CONTROLLERDEVICEREMOVED:
                break;
            default:
                break;
        }
    }

    return 0;
}

int LoveEvent::Poll(lua_State * L)
{
    if (poll_queue.empty())
    {
        lua_pushnil(L);

        return 1;
    }
    else
    {
        map<string, vector<Variant>> info = poll_queue.front();
        size_t size = 0;

        for (auto it = info.begin(); it != info.end(); ++it)
        {
            string event = it->first;
            size = it->second.size();

            lua_pushstring(L, event.c_str());
            
            for (uint index = 0; index < it->second.size(); index++)
            {
                Variant value = it->second[index];
                value.ToLua(L);
            }
        }

        //poll_queue.pop();

        return size + 1; //add event name
    }

    return 0;
}

//Löve2D Functions