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

std::array<touchPosition, MAX_TOUCH> Input::touches;

bool Input::PollEvent(LOVE_Event * event)
{
    hidScanInput();

    Input::down = hidKeysDown(CONTROLLER_P1_AUTO);
    Input::up = hidKeysUp(CONTROLLER_P1_AUTO);
    Input::held = hidKeysHeld(CONTROLLER_P1_AUTO);

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyDown<u64>() & it->second)
        {
            event->type = LOVE_GAMEPADDOWN;

            event->button.name = it->first;
            event->button.which = 0;

            return true;
        }
    }

    u32 touches = hidTouchCount();

    if (touches != prevTouchCount)
        prevTouchCount = touches;

    if (touches > 0)
    {
        for (u32 id = 0; id < touches; id++)
        {
            hidTouchRead(&Input::touches[id], id);

            if (Input::GetKeyDown<u64>() & KEY_TOUCH)
            {
                event->type = LOVE_TOUCHPRESS;

                event->touch.id = id;
                event->touch.x = Input::touches[id].px;
                event->touch.y = Input::touches[id].py;

                event->touch.dx = 0.0f;
                event->touch.dy = 0.0f;
                event->touch.pressure = 1.0f;

                return true;
            }
        }
    }

    // if (Input::GetKeyUp<u64>() & KEY_TOUCH)
    // {
    //     event->type = LOVE_TOUCHRELEASE;

    //     event->touch.id = prevTouchCount;
    //     event->touch.x = Input::touches[prevTouchCount].px;
    //     event->touch.y = Input::touches[prevTouchCount].py;
    //     event->touch.dx = 0.0f;
    //     event->touch.dy = 0.0f;
    //     event->touch.pressure = 1.0f;

    //     return true;
    // }

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyUp<u64>() & it->second)
        {
            event->type = LOVE_GAMEPADUP;

            event->button.name = it->first;
            event->button.which = 0;

            return true;
        }
    }

    JoystickPosition lStick;
    JoystickPosition rStick;

    if (lStick.dx != lastPosition[0].dx)
    {
        event->type = LOVE_GAMEPADAXIS;

        event->axis.axis = "leftx";
        event->axis.which = 0;
        event->axis.value = -1.0f;

        lastPosition[0].dx = lStick.dx;

        return true;
    }

    if (lStick.dy != lastPosition[0].dy)
    {
        event->type = LOVE_GAMEPADAXIS;

        event->axis.axis = "lefty";
        event->axis.which = 0;
        event->axis.value = -1.0f;

        lastPosition[0].dy = lStick.dy;

        return true;
    }

    if (rStick.dx != lastPosition[1].dx)
    {
        event->type = LOVE_GAMEPADAXIS;

        event->axis.axis = "rightx";
        event->axis.which = 0;
        event->axis.value = -1.0f;

        lastPosition[1].dx = rStick.dx;

        return true;
    }

    if (rStick.dy != lastPosition[1].dy)
    {
        event->type = LOVE_GAMEPADAXIS;

        event->axis.axis = "right";
        event->axis.which = 0;
        event->axis.value = -1.0f;

        lastPosition[1].dy = rStick.dy;

        return true;
    }

    return false;
}
