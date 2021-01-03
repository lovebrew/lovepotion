#include "objects/gamepad/gamepadc.h"

using namespace love::common;

love::Type Gamepad::type("Joystick", &Object::type);

Gamepad::Gamepad(size_t id) : id(id),
                              instanceID(-1),
                              vibration()
{}

float Gamepad::ClampValue(float x)
{
    if (std::abs(x) < 0.01)
        return 0.0f;

    if (x < -0.99f) return -1.0f;
    if (x > 0.99f) return 1.0f;

    return x;
}

std::string Gamepad::GetGUID() const
{
    return this->guid;
}

size_t Gamepad::GetInstanceID() const
{
    return this->instanceID;
}

size_t Gamepad::GetID() const
{
    return this->id;
}

bool Gamepad::IsGamepad() const
{
    return true;
}

bool Gamepad::GetConstant(const char * in, Gamepad::GamepadAxis & out)
{
    return axes.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::GamepadAxis in, const char *& out)
{
    return axes.Find(in, out);
}

bool Gamepad::GetConstant(const char * in, Gamepad::GamepadButton & out)
{
    return buttons.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::GamepadButton in, const char *& out)
{
    return buttons.Find(in, out);
}

bool Gamepad::GetConstant(const char * in, Gamepad::InputType & out)
{
    return inputTypes.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::InputType in, const char *& out)
{
    return inputTypes.Find(in, out);
}

StringMap<Gamepad::GamepadAxis, Gamepad::GAMEPAD_AXIS_MAX_ENUM>::Entry Gamepad::axisEntries[] =
{
    {"leftx",        GAMEPAD_AXIS_LEFTX},
    {"lefty",        GAMEPAD_AXIS_LEFTY},
    {"rightx",       GAMEPAD_AXIS_RIGHTX},
    {"righty",       GAMEPAD_AXIS_RIGHTY},
    {"triggerleft",  GAMEPAD_AXIS_TRIGGERLEFT},
    {"triggerright", GAMEPAD_AXIS_TRIGGERRIGHT},
};

StringMap<Gamepad::GamepadAxis, Gamepad::GAMEPAD_AXIS_MAX_ENUM> Gamepad::axes(Gamepad::axisEntries, sizeof(Gamepad::axisEntries));

StringMap<Gamepad::GamepadButton, Gamepad::GAMEPAD_BUTTON_MAX_ENUM>::Entry Gamepad::buttonEntries[] =
{
    {"a",             GAMEPAD_BUTTON_A},
    {"b",             GAMEPAD_BUTTON_B},
    {"x",             GAMEPAD_BUTTON_X},
    {"y",             GAMEPAD_BUTTON_Y},
    {"back",          GAMEPAD_BUTTON_BACK},
    {"guide",         GAMEPAD_BUTTON_GUIDE},
    {"start",         GAMEPAD_BUTTON_START},
    {"leftstick",     GAMEPAD_BUTTON_LEFTSTICK},
    {"rightstick",    GAMEPAD_BUTTON_RIGHTSTICK},
    {"leftshoulder",  GAMEPAD_BUTTON_LEFTSHOULDER},
    {"rightshoulder", GAMEPAD_BUTTON_RIGHTSHOULDER},
    {"dpup",          GAMEPAD_BUTTON_DPAD_UP},
    {"dpdown",        GAMEPAD_BUTTON_DPAD_DOWN},
    {"dpleft",        GAMEPAD_BUTTON_DPAD_LEFT},
    {"dpright",       GAMEPAD_BUTTON_DPAD_RIGHT},
};

StringMap<Gamepad::GamepadButton, Gamepad::GAMEPAD_BUTTON_MAX_ENUM> Gamepad::buttons(Gamepad::buttonEntries, sizeof(Gamepad::buttonEntries));

StringMap<Gamepad::InputType, Gamepad::INPUT_TYPE_MAX_ENUM>::Entry Gamepad::inputTypeEntries[] =
{
    {"axis",   Gamepad::INPUT_TYPE_AXIS},
    {"button", Gamepad::INPUT_TYPE_BUTTON},
};

StringMap<Gamepad::InputType, Gamepad::INPUT_TYPE_MAX_ENUM> Gamepad::inputTypes(Gamepad::inputTypeEntries, sizeof(Gamepad::inputTypeEntries));
