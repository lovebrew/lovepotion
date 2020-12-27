#include "driver/input.h"

std::unordered_map<const char *, int> Input::buttons =
{
    { "a",             HidNpadButton_A      }, { "b",             HidNpadButton_B      },
    { "dpright",       HidNpadButton_Right  }, { "dpleft",        HidNpadButton_Left   },
    { "dpdown",        HidNpadButton_Down   }, { "rightshoulder", HidNpadButton_R      },
    { "leftstick",     HidNpadButton_StickL }, { "rightstick",    HidNpadButton_StickR },
    { "start",         HidNpadButton_Plus   }, { "leftshoulder",  HidNpadButton_AnySL  },

    { "x",              HidNpadButton_X     }, { "y",             HidNpadButton_Y      },
    { "dpup",           HidNpadButton_Up    },
    { "leftshoulder",   HidNpadButton_L     },
    { "back",           HidNpadButton_Minus },
    { "rightshoulder",  HidNpadButton_AnySR }
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

#include "modules/joystick/joystick.h"

namespace
{
    static HidAnalogStickState sticks[2];
    static HidAnalogStickState oldSticks[2];

    static HidTouchScreenState touchState    = { 0 };

    static std::array<HidTouchState, MAX_TOUCH> stateTouches;
    static std::array<HidTouchState, MAX_TOUCH> oldStateTouches;
}

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
            auto & e = s_inputEvents.emplace_back();

            e.type = LOVE_GAMEPADDOWN;

            e.button.name = it->first;
            e.button.which  = 0;
        }
    }

    hidGetTouchScreenStates(&touchState, 1);
    int touchCount = touchState.count;

    if (touchCount > 0)
    {
        for (int id = 0; id < touchCount; id++)
        {
            auto & e = s_inputEvents.emplace_back();

            if (touchCount > prevTouchCount && id >= prevTouchCount)
            {
                stateTouches[id] = touchState.touches[id]; //< read the touches
                oldStateTouches[id] = stateTouches[id]; //< set old touches to newly read ones

                e.type = LOVE_TOUCHPRESS;
            }
            else
            {
                oldStateTouches[id] = stateTouches[id]; //< set old touches to currently read ones
                stateTouches[id] = touchState.touches[id]; //< read the touches

                e.type = LOVE_TOUCHMOVED;
            }

            e.touch.id = id;

            e.touch.x = stateTouches[id].x;
            e.touch.y = stateTouches[id].y;

            u32 currentX = (stateTouches[id].x > oldStateTouches[id].x) ? stateTouches[id].x : oldStateTouches[id].x;
            u32 currentY = (stateTouches[id].y > oldStateTouches[id].y) ? stateTouches[id].y : oldStateTouches[id].y;

            u32 prevX = (currentX == stateTouches[id].x) ? oldStateTouches[id].x : stateTouches[id].x;
            u32 prevY = (currentY == stateTouches[id].y) ? oldStateTouches[id].y : stateTouches[id].y;

            e.touch.dx = currentX - prevX;
            e.touch.dy = currentY - prevY;

            e.touch.pressure = 1.0f;

            if (e.type == LOVE_TOUCHMOVED && !e.touch.dx && !e.touch.dy)
            {
                s_inputEvents.pop_back();
                continue;
            }
        }
    }

    if (touchCount < prevTouchCount)
    {
        for (int id = 0; id < prevTouchCount; ++id)
        {
            auto & e = s_inputEvents.emplace_back();

            e.type = LOVE_TOUCHRELEASE;

            e.touch.id = id;
            e.touch.x = stateTouches[id].x;
            e.touch.y = stateTouches[id].y;
            e.touch.dx = 0.0f;
            e.touch.dy = 0.0f;
            e.touch.pressure = 0.0f;
        }
    }

    prevTouchCount = touchCount;

    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
        if (Input::GetKeyUp<u64>() & it->second)
        {
            auto & e = s_inputEvents.emplace_back();

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
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u64>() & KEY_ZL)
    {
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "triggerleft";
        e.axis.which = 0;
        e.axis.value = 0.0f;
    }

    /*  ZR / Right Trigger */

    if (Input::GetKeyDown<u64>() & KEY_ZR)
    {
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "triggerright";
        e.axis.which = 0;
        e.axis.value = 1.0f;
    }

    if (Input::GetKeyUp<u64>() & KEY_ZR)
    {
        auto & e = s_inputEvents.emplace_back();

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
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "leftx";
        e.axis.which = 0;
        e.axis.value = sticks[0].x / (float)JOYSTICK_MAX;

        oldSticks[0].x = sticks[0].x;
    }

    // LEFT Y AXIS
    if (oldSticks[0].y != sticks[0].y)
    {
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "lefty";
        e.axis.which = 0;
        e.axis.value = -(sticks[0].y / (float)JOYSTICK_MAX);

        oldSticks[0].y = sticks[0].y;
    }

    // RIGHT X AXIS
    if (oldSticks[1].x != sticks[1].x)
    {
        auto & e = s_inputEvents.emplace_back();

        e.type = LOVE_GAMEPADAXIS;

        e.axis.axis = "rightx";
        e.axis.which = 0;
        e.axis.value = sticks[1].x / (float)JOYSTICK_MAX;

        oldSticks[1].x = sticks[1].x;
    }

    // RIGHT Y AXIS
    if (oldSticks[1].y != sticks[1].y)
    {
        auto & e = s_inputEvents.emplace_back();

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
