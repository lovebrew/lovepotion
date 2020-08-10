#include "common/runtime.h"
#include "common/backend/input.h"

std::unordered_map<std::string, int> Input::buttons =
{
    { "a", KEY_A}, { "b", KEY_B }, { "x", KEY_X }, { "y", KEY_Y },
    { "dpright", KEY_DRIGHT}, { "dpleft", KEY_DLEFT },
    { "dpup", KEY_DUP }, { "dpdown", KEY_DDOWN }, { "rightshoulder", KEY_R },
    { "leftshoulder", KEY_L }, { "back", KEY_SELECT }, { "start", KEY_START}
};

static Input::JoystickState joystick[MAX_GAMEPADS];

void Input::CheckFocus(APT_HookType type, void * param)
{
    switch (type)
    {
        case APTHOOK_ONRESTORE:
        case APTHOOK_ONWAKEUP:
            Input::SendFocus(true);
            break;
        case APTHOOK_ONSUSPEND:
        case APTHOOK_ONSLEEP:
            Input::SendFocus(false);
            break;
        default:
            break;
    }
}

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

    hidTouchRead(&touches[0]);

    Input::down = hidKeysDown();
    Input::up = hidKeysUp();
    Input::held = hidKeysHeld();

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyDown<u32>() & it->second)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_GAMEPADDOWN;

            e.button.name = it->first;
            e.button.which = 0;
        }
    }

    if (!touchHeld && (Input::GetKeyDown<u32>() & KEY_TOUCH))
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_TOUCHPRESS;

        e.touch.id = 0;

        e.touch.x = touches[0].px;
        e.touch.y = touches[0].py;
        e.touch.dx = 0.0f;
        e.touch.dy = 0.0f;
        e.touch.pressure = 1.0f;

        Input::prevTouches[0] = Input::touches[0];
    }

    if (Input::GetKeyHeld<u32>() & KEY_TOUCH)
    {
        float dx = Input::prevTouches[0].px - touches[0].px;
        float dy = Input::prevTouches[0].py - touches[0].py;

        if (dx != 0 || dy != 0)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_TOUCHMOVED;

            e.touch.id = 0;

            e.touch.x = touches[0].px;
            e.touch.y = touches[0].py;

            e.touch.dx = dx;
            e.touch.dy = dy;

            e.touch.pressure = 1.0f;

            touchHeld = true;
        }
    }

    if (Input::GetKeyUp<u32>() & KEY_TOUCH)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_TOUCHRELEASE;

        e.touch.id = 0;

        e.touch.x = prevTouches[0].px;
        e.touch.y = prevTouches[0].py;

        e.touch.dx = 0.0f;
        e.touch.dy = 0.0f;

        e.touch.pressure = 0.0f;

        touchHeld = false;
    }

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyUp<u32>() & it->second)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_GAMEPADUP;

            e.button.name = it->first;
            e.button.which = 0;
        }
    }

    /* Gamepad Callbacks */

    /* ZL / Left Trigger */

    if (Input::GetKeyDown<u32>() & KEY_ZL)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u32>() & KEY_ZL)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 0.0f;
    }

    /*  ZR / Right Trigger */

    if (Input::GetKeyDown<u32>() & KEY_ZR)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerright";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u32>() & KEY_ZR)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerright";
        e.axis.which = 0;
        e.axis.value = 0.0f;
    }

    static JoystickState oldjoyStates[MAX_GAMEPADS];

    hidCircleRead(&joystick[0].left);
    irrstCstickRead(&joystick[0].right);

    // LEFT X AXIS
    if (oldjoyStates[0].left.dx != joystick[0].left.dx)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "leftx";
        e.axis.which = 0;
        e.axis.value = joystick[0].left.dx / JOYSTICK_MAX;

        oldjoyStates[0].left.dx = joystick[0].left.dx;
    }

    // LEFT Y AXIS
    if (oldjoyStates[0].left.dy != joystick[0].left.dy)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "lefty";
        e.axis.which = 0;
        e.axis.value = -(joystick[0].left.dy / JOYSTICK_MAX);

        oldjoyStates[0].left.dy = joystick[0].left.dy;
    }

    // RIGHT X AXIS
    if (oldjoyStates[0].right.dx != joystick[0].right.dx)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "rightx";
        e.axis.which = 0;
        e.axis.value = joystick[0].right.dx / JOYSTICK_MAX;

        oldjoyStates[0].right.dx = joystick[0].right.dx;
    }

    // RIGHT Y AXIS
    if (oldjoyStates[0].right.dy != joystick[0].right.dy)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "righty";
        e.axis.which = 0;
        e.axis.value = -(joystick[0].right.dy / JOYSTICK_MAX);

        oldjoyStates[0].right.dy = joystick[0].right.dy;
    }

    if (s_inputEvents.empty())
        return false;

    *event = s_inputEvents.front();
    s_inputEvents.pop_front();
    s_hysteresis = true;

    return true;
}
