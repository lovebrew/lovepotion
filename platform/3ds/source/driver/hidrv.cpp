#include "driver/hidrv.h"
#include "objects/gamepad/gamepad.h"

#include <string.h>

using namespace love::driver;

static constexpr std::array<Hidrv::ButtonMapping, 12> mappings = { {
    { "a", KEY_A, 1 },
    { "b", KEY_B, 2 },
    { "x", KEY_X, 3 },
    { "y", KEY_Y, 4 },

    { "leftshoulder", KEY_L, 5 },
    { "rightshoulder", KEY_R, 6 },

    { "back", KEY_SELECT, 7 },
    { "start", KEY_START, 8 },

    { "dpright", KEY_DRIGHT, -1 },
    { "dpleft", KEY_DLEFT, -1 },
    { "dpup", KEY_DUP, -1 },
    { "dpdown", KEY_DDOWN, -1 },
} };

Hidrv::Hidrv() : oldSticks {}, oldTouchState {}, touchHeld(false)
{}

bool Hidrv::IsDown(size_t button)
{
    size_t index = std::clamp((int)button - 1, 0, 7);
    return this->buttonStates.held & mappings[index].key;
}

bool Hidrv::Poll(LOVE_Event* event)
{
    if (!this->events.empty())
    {
        *event = this->events.front();
        this->events.pop_front();

        return true;
    }

    if (this->hysteresis)
        return this->hysteresis = false;

    hidScanInput();

    /* handle button inputs */

    this->buttonStates.pressed  = hidKeysDown();
    this->buttonStates.released = hidKeysUp();
    this->buttonStates.held     = hidKeysHeld();

    for (auto& mapping : mappings)
    {
        if (this->buttonStates.pressed != this->buttonStates.oldPressed &&
            (this->buttonStates.pressed & mapping.key))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADDOWN;

            newEvent.button.name   = mapping.name;
            newEvent.button.which  = 0;
            newEvent.button.button = mapping.index;
        }
    }

    for (auto& mapping : mappings)
    {
        if (this->buttonStates.released != this->buttonStates.oldReleased &&
            (this->buttonStates.released & mapping.key))
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADUP;

            newEvent.button.name   = mapping.name;
            newEvent.button.which  = 0;
            newEvent.button.button = mapping.index;
        }
    }

    this->buttonStates.oldPressed  = this->buttonStates.pressed;
    this->buttonStates.oldReleased = this->buttonStates.released;

    /* touch screen */

    hidTouchRead(&this->touchState);

    if (!this->touchHeld && (this->buttonStates.pressed & KEY_TOUCH))
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_TOUCHPRESS;

        newEvent.touch.id       = 0;
        newEvent.touch.x        = this->touchState.px;
        newEvent.touch.y        = this->touchState.py;
        newEvent.touch.dx       = 0.0f;
        newEvent.touch.dy       = 0.0f;
        newEvent.touch.pressure = 1.0f;

        this->oldTouchState = this->touchState;
    }

    if (this->buttonStates.held & KEY_TOUCH)
    {
        float dx = this->oldTouchState.px - this->touchState.px;
        float dy = this->oldTouchState.py - this->touchState.py;

        if (dx != 0.0f || dy != 0.0f)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_TOUCHMOVED;

            newEvent.touch.id       = 0;
            newEvent.touch.x        = this->touchState.px;
            newEvent.touch.y        = this->touchState.py;
            newEvent.touch.dx       = dx;
            newEvent.touch.dy       = dy;
            newEvent.touch.pressure = 1.0f;

            this->touchHeld = true;
        }
    }

    if (this->buttonStates.released & KEY_TOUCH)
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_TOUCHRELEASE;

        newEvent.touch.id       = 0;
        newEvent.touch.x        = this->oldTouchState.px;
        newEvent.touch.y        = this->oldTouchState.py;
        newEvent.touch.dx       = 0.0f;
        newEvent.touch.dy       = 0.0f;
        newEvent.touch.pressure = 0.0f;

        this->oldTouchState = this->touchState;
    }

    /* axes */

    if ((this->buttonStates.pressed & KEY_ZL) || (this->buttonStates.released & KEY_ZL))
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_GAMEPADAXIS;

        newEvent.axis.which  = 0;
        newEvent.axis.axis   = "triggerleft";
        newEvent.axis.number = 3;

        float value         = (this->buttonStates.pressed & KEY_ZL) ? 1.0f : 0.0f;
        newEvent.axis.value = value;
    }

    if ((this->buttonStates.pressed & KEY_ZR) || (this->buttonStates.released & KEY_ZR))
    {
        auto& newEvent = this->events.emplace_back();

        newEvent.type = TYPE_GAMEPADAXIS;

        newEvent.axis.which  = 0;
        newEvent.axis.axis   = "triggerright";
        newEvent.axis.number = 6;

        float value         = (this->buttonStates.pressed & KEY_ZR) ? 1.0f : 0.0f;
        newEvent.axis.value = value;
    }

    /* handle stick inputs */

    hidCircleRead(&this->sticks[0]);
    irrstCstickRead(&this->sticks[1]);

    for (size_t index = 0; index < 2; index++)
    {
        if (this->oldSticks[index].dx != this->sticks[index].dx)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADAXIS;

            newEvent.axis.which = 0;

            newEvent.axis.number = (index == 1) ? 5 : 2;
            newEvent.axis.axis   = (index == 1) ? "rightx" : "leftx";
            newEvent.axis.value  = this->sticks[index].dx / Gamepad::JOYSTICK_MAX;

            oldSticks[index].dx = sticks[index].dx;
        }

        if (this->oldSticks[index].dy != this->sticks[index].dy)
        {
            auto& newEvent = this->events.emplace_back();

            newEvent.type = TYPE_GAMEPADAXIS;

            newEvent.axis.which = 0;

            newEvent.axis.number = (index == 1) ? 4 : 1;
            newEvent.axis.axis   = (index == 1) ? "righty" : "lefty";
            newEvent.axis.value  = -(this->sticks[index].dy / Gamepad::JOYSTICK_MAX);

            oldSticks[index].dy = sticks[index].dy;
        }
    }

    if (this->events.empty())
        return false;

    *event = this->events.front();
    this->events.pop_front();

    return this->hysteresis = true;
}