#pragma once

#include <string>

#include <stddef.h>

namespace love
{
    struct GamepadButton
    {
        size_t id;
        std::string name;

        int button;
    };

    struct GamepadAxis
    {
        size_t id;
        std::string name;

        size_t axis;
        float value;
    };

    struct Finger
    {
        int64_t id;

        double x;
        double y;

        double dx;
        double dy;

        double pressure;
    };

    struct Resize
    {
        int width;
        int height;
    };

    struct KeyboardInput
    {
        std::string text;
    };

    enum EventType
    {
        TYPE_GENERAL,
        TYPE_WINDOW,
        TYPE_TOUCH,
        TYPE_GAMEPAD,
        TYPE_KEYBOARD
    };

    enum SubEventType
    {
        SUBTYPE_GAMEPADAXIS,
        SUBTYPE_GAMEPADDOWN,
        SUBTYPE_GAMEPADUP,

        SUBTYPE_GAMEPADADDED,
        SUBTYPE_GAMEPADREMOVED,

        SUBTYPE_GAMEPADUPDATED,
        SUBTYPE_GAMEPADSENSORUPDATED,

        SUBTYPE_TOUCHPRESS,
        SUBTYPE_TOUCHRELEASE,
        SUBTYPE_TOUCHMOVED,

        SUBTYPE_TEXTINPUT,

        SUBTYPE_LOWMEMORY,

        SUBTYPE_FOCUS_GAINED,
        SUBTYPE_FOCUS_LOST,

        SUBTYPE_RESIZE,

        SUBTYPE_QUIT
    };

    struct LOVE_Event
    {
        EventType type;
        SubEventType subtype;

        GamepadButton gamepadButton;
        GamepadAxis gamepadAxis;

        Finger finger;

        Resize resize;

        KeyboardInput keyboardInput;
    };
} // namespace love
