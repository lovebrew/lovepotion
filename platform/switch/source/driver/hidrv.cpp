#include "driver/hidrv.h"
#include "objects/gamepad/gamepad.h"

#include "modules/joystick/joystick.h"

#include <string.h>

using namespace love::driver;

#define MODULE() love::Module::GetInstance<love::Joystick>(love::Module::M_JOYSTICK)

static constexpr std::array<Hidrv::ButtonMapping, 16> mappings =
{
    {
        { "a", HidNpadButton_A, 1 },
        { "b", HidNpadButton_B, 2 },
        { "x", HidNpadButton_X, 3 },
        { "y", HidNpadButton_Y, 4 },

        { "leftshoulder",  HidNpadButton_L, 5 },
        { "rightshoulder", HidNpadButton_R, 6 },

        { "back",  HidNpadButton_Minus, 7 },
        { "start", HidNpadButton_Plus,  8 },

        { "leftstick",  HidNpadButton_StickL, 9  },
        { "rightstick", HidNpadButton_StickR, 10 },

        { "dpright", HidNpadButton_Right, -1 },
        { "dpleft",  HidNpadButton_Left,  -1 },
        { "dpup",    HidNpadButton_Up,    -1 },
        { "dpdown",  HidNpadButton_Down,  -1 },

        { "leftshoulder",  HidNpadButton_AnySL, 5 },
        { "rightshoulder", HidNpadButton_AnySR, 6 }
    }
};

#include "common/debug/logger.h"

Hidrv::Hidrv() : sticks{},
                 oldSticks{},
                 touchState{},
                 oldStateTouches{},
                 currentPad{},
                 currentPadIndex(0),
                 prevTouchCount(0)
{}

bool Hidrv::IsDown(size_t button)
{
    size_t index = std::clamp((int)button - 1, 0, 9);
    return this->buttonHeld & mappings[index].key;
}

void Hidrv::CheckFocus()
{
    bool focused = (appletGetFocusState() == AppletFocusState_InFocus);

    uint32_t message = 0;
    Result res = appletGetMessage(&message);

    if (R_SUCCEEDED(res))
    {
        bool shouldClose = !appletProcessMessage(message);

        if (shouldClose)
        {
            this->SendQuit();
            return;
        }

        switch (message)
        {
            case AppletMessage_FocusStateChanged:
            {

                bool oldFocus = focused;
                AppletFocusState state = appletGetFocusState();

                focused = (state == AppletFocusState_InFocus);

                this->SendFocus(focused);

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
                    this->SendResize(1280, 720);
                else if (mode == AppletOperationMode_Console)
                    this->SendResize(1920, 1080);

                break;
            }
            default:
                break;
        }
    }
}

bool Hidrv::Poll(LOVE_Event * event)
{
    if (!this->events.empty())
    {
        *event = this->events.front();
        this->events.pop_front();

        return true;
    }

    if (this->hysteresis)
        return this->hysteresis = false;

    /* applet focus handling */
    this->CheckFocus();

    if (!MODULE())
        return false;

    if (this->currentPadIndex > 3)
        this->currentPadIndex = 0;

    Gamepad * gamepad = MODULE()->GetJoystickFromID(this->currentPadIndex);
    bool connected = false;

    if (gamepad != nullptr)
    {
        bool initialStatus = gamepad->IsConnected();

        this->currentPad = gamepad->GetPadState();
        padUpdate(&this->currentPad);

        connected = gamepad->IsConnected();

        /* connection status changed */
        if (connected != initialStatus)
        {
            auto & newEvent = this->events.emplace_back();

            newEvent.type = (connected) ? TYPE_GAMEPADADDED : TYPE_GAMEPADREMOVED;

            newEvent.padStatus.which = gamepad->GetID();
            newEvent.padStatus.connected = connected;
        }
    }

    /* touch screen */
    hidGetTouchScreenStates(&touchState, 1);
    int touchCount = touchState.count;

    if (touchCount > 0)
    {
        for (int id = 0; id < touchCount; id++)
        {
            auto & newEvent = this->events.emplace_back();

            if (touchCount > this->prevTouchCount && id >= this->prevTouchCount)
            {
                this->stateTouches[id]    = this->touchState.touches[id]; //< read the touches
                this->oldStateTouches[id] = this->stateTouches[id]; //< set old touches to newly read ones

                newEvent.type = TYPE_TOUCHPRESS;
            }
            else
            {
                this->oldStateTouches[id] = this->stateTouches[id]; //< set old touches to currently read ones
                this->stateTouches[id]    = this->touchState.touches[id]; //< read the touches

                newEvent.type = TYPE_TOUCHMOVED;
            }

            newEvent.touch.id = id;

            newEvent.touch.x = this->stateTouches[id].x;
            newEvent.touch.y = this->stateTouches[id].y;

            newEvent.touch.dx = (s32)this->stateTouches[id].x - this->oldStateTouches[id].x;
            newEvent.touch.dy = (s32)this->stateTouches[id].y - this->oldStateTouches[id].y;

            newEvent.touch.pressure = 1.0f;

            if (newEvent.type == TYPE_TOUCHMOVED && !newEvent.touch.dx && !newEvent.touch.dy)
            {
                this->events.pop_back();
                continue;
            }
        }
    }

    if (touchCount < this->prevTouchCount)
    {
        for (int id = 0; id < prevTouchCount; ++id)
        {
            auto & newEvent = this->events.emplace_back();

            newEvent.type = TYPE_TOUCHRELEASE;

            newEvent.touch.id = id;
            newEvent.touch.x = this->stateTouches[id].x;
            newEvent.touch.y = this->stateTouches[id].y;
            newEvent.touch.dx = 0.0f;
            newEvent.touch.dy = 0.0f;
            newEvent.touch.pressure = 0.0f;
        }
    }

    this->prevTouchCount = touchCount;

    /* handle button inputs */
    if (gamepad && gamepad->IsConnected())
    {
        this->buttonPressed  = padGetButtonsDown(&this->currentPad);
        this->buttonReleased = padGetButtonsUp(&this->currentPad);
        this->buttonHeld     = padGetButtons(&this->currentPad);

        for (auto & mapping : mappings)
        {
            if (this->buttonPressed & mapping.key)
            {
                auto & newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADDOWN;

                newEvent.button.name   = mapping.name;
                newEvent.button.which  = gamepad->GetID();
                newEvent.button.button = mapping.index;
            }
        }

        for (auto & mapping : mappings)
        {
            if (this->buttonReleased & mapping.key)
            {
                auto & newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADUP;

                newEvent.button.name   = mapping.name;
                newEvent.button.which  = gamepad->GetID();
                newEvent.button.button = mapping.index;
            }
        }

        /* axes */
        if ((this->buttonPressed & HidNpadButton_ZL) || (this->buttonReleased & HidNpadButton_ZL))
        {
            auto & newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADAXIS;

            newEvent.axis.which = gamepad->GetID();
            newEvent.axis.axis  = "triggerleft";
            newEvent.axis.number = 3;

            float value = (this->buttonPressed & HidNpadButton_ZL) ? 1.0f : 0.0f;
            newEvent.axis.value = value;
        }

        if ((this->buttonPressed & HidNpadButton_ZR) || (this->buttonReleased & HidNpadButton_ZR))
        {
            auto & newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADAXIS;

            newEvent.axis.which = gamepad->GetID();
            newEvent.axis.axis  = "triggerright";
            newEvent.axis.number = 6;

            float value = (this->buttonPressed & KEY_ZR) ? 1.0f : 0.0f;
            newEvent.axis.value = value;
        }

        /* handle stick inputs */
        for (size_t index = 0; index < 2; index++)
        {
            this->sticks[index] = padGetStickPos(&this->currentPad, index);

            if (this->oldSticks[index].x != this->sticks[index].x)
            {
                auto & newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADAXIS;

                newEvent.axis.which = gamepad->GetID();

                newEvent.axis.number = (index == 1) ? 5 : 2;
                newEvent.axis.axis = (index == 1) ? "rightx" : "leftx";
                newEvent.axis.value = this->sticks[index].x / JOYSTICK_MAX;

                oldSticks[index].x = sticks[index].x;
            }

            if (this->oldSticks[index].y != this->sticks[index].y)
            {
                auto & newEvent = this->events.emplace_back();

                newEvent.type = TYPE_GAMEPADAXIS;

                newEvent.axis.which = gamepad->GetID();

                newEvent.axis.number = (index == 1) ? 4 : 1;
                newEvent.axis.axis = (index == 1) ? "righty" : "lefty";
                newEvent.axis.value = -(this->sticks[index].y / JOYSTICK_MAX);

                oldSticks[index].y = sticks[index].y;
            }
        }
    }

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    // this->currentPadIndex++;

    return this->hysteresis = true;
}