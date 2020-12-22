#include "common/runtime.h"
#include "common/backend/input.h"

std::unordered_map<std::string, int> Input::buttons =
{
    { "a", KEY_A }, { "b", KEY_B }, { "x", KEY_X }, { "y", KEY_Y },
    { "dpright", KEY_DRIGHT }, { "dpleft", KEY_DLEFT }, { "dpup", KEY_DUP },
    { "dpdown", KEY_DDOWN }, { "rightshoulder", KEY_R }, { "leftshoulder", KEY_L },
    { "leftstick", KEY_LSTICK }, { "rightstick", KEY_RSTICK }, { "back", KEY_MINUS },
    { "start", KEY_PLUS}, { "leftshoulder", KEY_SL }, { "rightshoulder", KEY_SR }
};

void Input::CheckFocus()
{
    bool focused = (appletGetFocusState() == AppletFocusState_InFocus);

    u32 message = 0;
    Result res = appletGetMessage(&message);

    if (R_SUCCEEDED(res))
    {
        bool shouldClose = !appletProcessMessage(message);

        if (shouldClose)
        {
            Input::SendQuit();
            return;
        }

        switch (message)
        {
            case AppletMessage_FocusStateChanged:
            {
                bool oldFocus = focused;
                AppletFocusState state = appletGetFocusState();
                focused = (state == AppletFocusState_InFocus);

                Input::SendFocus(focused);

                // don't want focus to be the same
                if (focused == oldFocus)
                    break;

                if (focused)
                    appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
                else
                    appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleepNotify);

                break;
            }
            case AppletMessage_OperationModeChanged:
            {
                AppletOperationMode mode = appletGetOperationMode();

                if (mode == AppletOperationMode_Handheld)
                    Input::SendResize(1280, 720);
                else if (mode == AppletOperationMode_Console)
                    Input::SendResize(1920, 1080);

                break;
            }
            default:
                break;
        }
    }
}

#include "modules/joystick/joystickc.h"

static HidAnalogStickState sticks[2];
static HidAnalogStickState oldSticks[2];

static HidTouchScreenState touchState = { 0 };
static HidTouchScreenState oldTouchState = { 0 };

#define MODULE() love::Module::GetInstance<love::Joystick>(love::Module::M_JOYSTICK)

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

    if (!MODULE())
        return false;

    PadState & pad = MODULE()->GetJoystickFromID(0)->GetPadState();

    padUpdate(&pad);

    Input::down = padGetButtonsDown(&pad);
    Input::up = padGetButtonsUp(&pad);
    Input::held = padGetButtons(&pad);

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

    hidGetTouchScreenStates(&touchState, 1);

    if (touchState.count > 0)
    {
        for (int id = 0; id < touchState.count; id++)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            if (touchState.count > prevTouchCount && id >= prevTouchCount)
            {
                oldTouchState = touchState;
                e.type = LOVE_TOUCHPRESS;
            }
            else
            {
                oldTouchState = touchState;
                e.type = LOVE_TOUCHMOVED;
            }

            e.touch.id = id;

            e.touch.x = touchState.touches[id].x;
            e.touch.y = touchState.touches[id].y;

            float dx = touchState.touches[id].x - oldTouchState.touches[id].x;
            float dy = touchState.touches[id].y - oldTouchState.touches[id].y;

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

    if (oldTouchState.count < prevTouchCount)
    {
        for (int id = 0; id < prevTouchCount; ++id)
        {
            s_inputEvents.emplace_back();
            auto & e = s_inputEvents.back();

            e.type = LOVE_TOUCHRELEASE;

            e.touch.id = id;
            e.touch.x = oldTouchState.touches[id].x;
            e.touch.y = oldTouchState.touches[id].y;
            e.touch.dx = 0.0f;
            e.touch.dy = 0.0f;
            e.touch.pressure = 0.0f;
        }
    }

    prevTouchCount = oldTouchState.count;

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

    /* Applet Focus Handling */
    Input::CheckFocus();

    /* ZL / Left Trigger */
    if (Input::GetKeyDown<u64>() & KEY_ZL)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u64>() & KEY_ZL)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 0.0f;
    }

    /*  ZR / Right Trigger */

    if (Input::GetKeyDown<u64>() & KEY_ZR)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerright";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u64>() & KEY_ZR)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;
        e.axis.axis = "triggerright";
        e.axis.which = 0;
        e.axis.value = 0.0f;
    }

    /* Joystick Values */
    sticks[0] = padGetStickPos(&pad, 0);
    sticks[1] = padGetStickPos(&pad, 1);

    // LEFT X AXIS
    if (oldSticks[0].x != sticks[0].x)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "leftx";
        e.axis.which = 0;
        e.axis.value = sticks[0].x / (float)JOYSTICK_MAX;

        oldSticks[0].x = sticks[0].x;
    }

    // LEFT Y AXIS
    if (oldSticks[0].y != sticks[0].y)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "lefty";
        e.axis.which = 0;
        e.axis.value = -(sticks[0].y / (float)JOYSTICK_MAX);

        oldSticks[0].y = sticks[0].y;
    }

    // RIGHT X AXIS
    if (oldSticks[1].x != sticks[1].x)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "rightx";
        e.axis.which = 0;
        e.axis.value = sticks[1].x / (float)JOYSTICK_MAX;

        oldSticks[1].x = sticks[1].x;
    }

    // RIGHT Y AXIS
    if (oldSticks[1].y != sticks[1].y)
    {
        s_inputEvents.emplace_back();
        auto & e = s_inputEvents.back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "righty";
        e.axis.which = 0;
        e.axis.value = -(sticks[1].y / (float)JOYSTICK_MAX);

        oldSticks[1].y = sticks[1].y;
    }

    if (s_inputEvents.empty())
        return false;

    *event = s_inputEvents.front();
    s_inputEvents.pop_front();
    s_hysteresis = true;

    return true;
}
