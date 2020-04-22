#include "common/runtime.h"
#include "common/backend/input.h"

std::unordered_map<std::string, int> Input::buttons =
{
    { "a", KEY_A }, { "b", KEY_B }, { "x", KEY_X }, { "y", KEY_Y },
    { "dpright", KEY_DRIGHT }, { "dpleft", KEY_DLEFT }, { "dpup", KEY_DUP },
    { "dpdown", KEY_DDOWN }, { "rightshoulder", KEY_R }, { "leftshoulder", KEY_L },
    { "leftstick", KEY_LSTICK }, { "rightstick", KEY_RSTICK }, { "back", KEY_MINUS },
    { "start", KEY_PLUS }
};

bool Input::PollEvent(LOVE_Event * event)
{
    if (!s_inputEvents.empty())
    {
        *event = s_inputEvents.front();
        s_inputEvents.pop_front();

        return true;
    }

    if (s_hysteresis)
    {
        s_hysteresis = false;
        return false;
    }

    hidScanInput();

    Input::down = hidKeysDown(CONTROLLER_P1_AUTO);
    Input::up = hidKeysUp(CONTROLLER_P1_AUTO);
    Input::held = hidKeysHeld(CONTROLLER_P1_AUTO);

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyDown<u64>() & it->second)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_GAMEPADDOWN;
            e.button.name = it->first;
            e.button.which = 0;
        }
    }

    u32 touches = hidTouchCount();

    if (touches > 0)
    {
        for (u32 id = 0; id < touches; id++)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            if (touches > prevTouchCount && id >= prevTouchCount)
            {
                hidTouchRead(&Input::touches[id], id);
                Input::prevTouches[id] = Input::touches[id];

                e.type = LOVE_TOUCHPRESS;
            }
            else
            {
                Input::prevTouches[id] = Input::touches[id];
                hidTouchRead(&Input::touches[id], id);

                e.type = LOVE_TOUCHMOVED;
            }

            e.touch.id = id;

            e.touch.x = Input::touches[id].px;
            e.touch.y = Input::touches[id].py;

            float dx = Input::touches[id].px - Input::prevTouches[id].px;
            float dy = Input::touches[id].py - Input::prevTouches[id].py;

            e.touch.dx = dx;
            e.touch.dy = dy;

            e.touch.pressure = 1.0f;

            if (e.type == LOVE_TOUCHMOVED && !e.touch.dx && !e.touch.dy)
            {
                s_inputEvents.pop_back();
                continue;
            }
        }
    }

    if (touches < prevTouchCount)
    {
        for (u32 id = 0; id < prevTouchCount; ++id)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_TOUCHRELEASE;

            e.touch.id = id;
            e.touch.x = Input::touches[id].px;
            e.touch.y = Input::touches[id].py;
            e.touch.dx = 0.0f;
            e.touch.dy = 0.0f;
            e.touch.pressure = 0.0f;
        }
    }

    prevTouchCount = touches;

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyUp<u64>() & it->second)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_GAMEPADUP;

            e.button.name = it->first;
            e.button.which = 0;
        }
    }

    JoystickPosition lStick;
    JoystickPosition rStick;

    hidJoystickRead(&lStick, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
    hidJoystickRead(&rStick, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);

    if (lStick.dx != lastPosition[0].dx)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "leftx";
        e.axis.which = 0;
        e.axis.value = -1.0f;

        lastPosition[0].dx = lStick.dx;
    }

    if (lStick.dy != lastPosition[0].dy)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "lefty";
        e.axis.which = 0;
        e.axis.value = -1.0f;

        lastPosition[0].dy = lStick.dy;
    }

    if (rStick.dx != lastPosition[1].dx)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "rightx";
        e.axis.which = 0;
        e.axis.value = -1.0f;

        lastPosition[1].dx = rStick.dx;
    }

    if (rStick.dy != lastPosition[1].dy)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "right";
        e.axis.which = 0;
        e.axis.value = -1.0f;

        lastPosition[1].dy = rStick.dy;
    }

    if (s_inputEvents.empty())
        return false;

    *event = s_inputEvents.front();
    s_inputEvents.pop_front();
    s_hysteresis = true;

    return true;
}
