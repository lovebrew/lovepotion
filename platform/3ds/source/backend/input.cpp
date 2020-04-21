#include "common/runtime.h"
#include "common/backend/input.h"

std::unordered_map<std::string, int> Input::buttons =
{
    { "a", KEY_A}, { "b", KEY_B }, { "x", KEY_X }, { "y", KEY_Y },
    { "dpright", KEY_DRIGHT}, { "dpleft", KEY_DLEFT },
    { "dpup", KEY_DUP }, { "dpdown", KEY_DDOWN }, { "rightshoulder", KEY_R },
    { "leftshoulder", KEY_L }, { "back", KEY_SELECT }, { "start", KEY_START}
};

std::array<touchPosition, MAX_TOUCH> Input::touches;

bool Input::PollEvent(LOVE_Event * event)
{
    hidScanInput();

    hidTouchRead(&touches[0]);

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

        event->touch.x = touches[0].px;
        event->touch.y = touches[0].py;
        event->touch.dx = 0.0f;
        event->touch.dy = 0.0f;
        event->touch.pressure = 1.0f;

        lastTouch = { touches[0].px, touches[0].py };

        return true;
    }

    u16 dx = std::abs(lastTouch.px - touches[0].px);
    u16 dy = std::abs(lastTouch.py - touches[0].py);

    if (dx > 0 || dy > 0)
    {
        if (Input::GetKeyHeld<u32>() & KEY_TOUCH)
        {
            event->type = LOVE_TOUCHMOVED;

            event->touch.id = 0;

            lastTouch = { touches[0].px, touches[0].py };

            event->touch.x = touches[0].px;
            event->touch.y = touches[0].py;
            event->touch.dx = lastTouch.px - touches[0].px;
            event->touch.dy = lastTouch.py - touches[0].py;
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
        event->touch.x = touches[0].px;
        event->touch.y = touches[0].py;
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
