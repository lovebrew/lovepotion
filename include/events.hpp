#pragma once

#include <string>

#include <stddef.h>
#include <stdint.h>

#include <vector>

namespace love
{
    struct GamepadButton
    {
        int which;
        int button;
    };

    struct GamepadAxis
    {
        int which;
        int axis;
        float value;
    };

    struct GamepadSensor
    {
        int which;
        int sensor;
        std::vector<float> data;
    };

    struct GamepadStatus
    {
        bool added;
        int which;
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
        SUBTYPE_JOYSTICKAXIS,
        SUBTYPE_GAMEPADDOWN,
        SUBTYPE_JOYSTICKDOWN,
        SUBTYPE_GAMEPADUP,
        SUBTYPE_JOYSTICKUP,

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
        GamepadSensor gamepadSensor;

        GamepadStatus gamepadStatus;

        Finger finger;

        Resize resize;

        KeyboardInput keyboardInput;
    };
} // namespace love
