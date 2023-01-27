#pragma once

#include <stddef.h>
#include <string_view>

#include <common/vector.hpp>

#include <modules/sensor/sensor.hpp>

namespace love
{
    struct GamepadButton
    {
        size_t id; //< Gamepad ID

        const char* name; //< Button name
        int button;       //< Button number
    };

    struct GamepadAxis
    {
        size_t id; //< Gamepad ID

        size_t axis;      //< Axis number
        const char* name; //< Axis name
        float value;      //< Axis value
    };

    struct GamepadStatus
    {
        size_t id; //< Gamepad ID
    };

    struct GamepadSensor
    {
        size_t id;

        Sensor::SensorType type;
        std::array<float, 3> data;
    };

    struct Finger
    {
        int64_t id; //< Touch ID

        double x; //< Touch x-coordinate
        double y; //< Touch y-coordinate

        double dx; //< Touch movement (x-axis)
        double dy; //< Touch movement (y-axis)

        double pressure; //< Touch pressue (always 0 or 1)
    };

    struct Resize
    {
        int width;  //< Screen width
        int height; //< Screen height
    };

    struct KeyboardInput
    {
        std::string text;
    };

    enum EventType
    {
        TYPE_WINDOW,
        TYPE_TOUCH,
        TYPE_GAMEPAD,
        TYPE_GENERAL,
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
        SubEventType subType;

        GamepadStatus padStatus;
        GamepadButton padButton;
        GamepadAxis padAxis;
        GamepadSensor padSensor;

        Finger touchFinger;
        Resize size;
        KeyboardInput keyboard;
    };
} // namespace love
