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
    hidScanInput();
    touchPosition touch;

    Input::down = hidKeysDown(CONTROLLER_P1_AUTO);
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

    // TODO: handle multiple touches
    if (Input::GetKeyDown<u64>() & KEY_TOUCH)
    {
        hidTouchRead(&touch, 0);

        event->type = LOVE_TOUCHPRESS;
        event->touch.id = 0;

        event->touch.x = touch.px;
        event->touch.y = touch.py;

        lastTouch[0] = touch.px;
        lastTouch[1] = touch.py;

        return true;
    }

    Input::up = hidKeysUp(CONTROLLER_P1_AUTO);
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

    // TODO: handle multiple touches
    if (Input::GetKeyUp<u64>() & KEY_TOUCH)
    {
        event->type = LOVE_TOUCHRELEASE;
        event->touch.id = 0;

        event->touch.x = lastTouch[0];
        event->touch.y = lastTouch[1];

        return true;
    }

    return false;
}
