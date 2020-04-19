#include "common/runtime.h"
#include "common/backend/input.h"

std::unordered_map<std::string, int> Input::buttons =
{
    { "a", KEY_A}, { "b", KEY_B }, { "x", KEY_X }, { "y", KEY_Y },
    { "dpright", KEY_DRIGHT}, { "dpleft", KEY_DLEFT },
    { "dpup", KEY_DUP }, { "dpdown", KEY_DDOWN }, { "rightshoulder", KEY_R },
    { "leftshoulder", KEY_L }, { "back", KEY_SELECT }, { "start", KEY_START}
};

bool Input::PollEvent(LOVE_Event * event)
{
    hidScanInput();

    touchPosition touch;
    hidTouchRead(&touch);

    Input::down = hidKeysDown();
    Input::up = hidKeysUp();
    Input::held = hidKeysHeld();

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyDown<u32>() & it->second)
        {
            event->type = LOVE_GAMEPADDOWN;

            event->button.name = it->first;
            event->button.which = 0;

            return true;
        }
    }

    if (!touchHeld && (Input::GetKeyDown<u32>() & KEY_TOUCH))
    {
        event->type = LOVE_TOUCHPRESS;

        event->touch.id = 0;

        event->touch.x = touch.px;
        event->touch.y = touch.py;
        event->touch.dx = 0.0f;
        event->touch.dy = 0.0f;
        event->touch.pressure = 1.0f;

        lastTouch[0] = touch.px;
        lastTouch[1] = touch.py;

        return true;
    }

    if (std::abs(lastTouch[0] - touch.px) > 0 || std::abs(lastTouch[1] - touch.py) > 0)
    {
        if (Input::GetKeyHeld<u32>() & KEY_TOUCH)
        {
            event->type = LOVE_TOUCHMOVED;

            event->touch.id = 0;

            lastTouch[0] = touch.px;
            lastTouch[1] = touch.py;

            event->touch.x = touch.px;
            event->touch.y = touch.py;
            event->touch.dx = lastTouch[0] - touch.px;
            event->touch.dy = lastTouch[1] - touch.py;
            event->touch.pressure = 1.0f;

            touchHeld = true;

            return true;
        }
    }

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyUp<u32>() & it->second)
        {
            event->type = LOVE_GAMEPADUP;

            event->button.name = it->first;
            event->button.which = 0;

            return true;
        }
    }

    if (Input::GetKeyUp<u32>() & KEY_TOUCH)
    {
        event->type = LOVE_TOUCHRELEASE;

        event->touch.id = 0;
        event->touch.x = lastTouch[0];
        event->touch.y = lastTouch[1];
        event->touch.dx = 0.0f;
        event->touch.dy = 0.0f;
        event->touch.pressure = 1.0f;

        touchHeld = false;

        return true;
    }

    /* Gamepad Callbacks */

    circlePosition lStick;
    circlePosition rStick;

    hidCircleRead(&lStick);
    irrstCstickRead(&rStick);

    /* ZL / Left Trigger */

    if (Input::GetKeyDown<u32>() & KEY_ZL)
    {
        event->type = LOVE_GAMEPADAXIS;
        event->axis.axis = "triggerleft";
        event->axis.which = 0;
        event->axis.value = 1.0f;

        return true;
    }

    if (Input::GetKeyUp<u32>() & KEY_ZL)
    {
        event->type = LOVE_GAMEPADAXIS;
        event->axis.axis = "triggerleft";
        event->axis.which = 0;
        event->axis.value = 0.0f;

        return true;
    }

    /*  ZR / Right Trigger */

    if (Input::GetKeyDown<u32>() & KEY_ZR)
    {
        event->type = LOVE_GAMEPADAXIS;
        event->axis.axis = "triggerright";
        event->axis.which = 0;
        event->axis.value = 1.0f;

        return true;
    }

    if (Input::GetKeyUp<u32>() & KEY_ZR)
    {
        event->type = LOVE_GAMEPADAXIS;
        event->axis.axis = "triggerright";
        event->axis.which = 0;
        event->axis.value = 0.0f;

        return true;
    }

    /* Left Stick */

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

    /* Right Stick */

    return false;
}
