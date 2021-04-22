#include "objects/gamepad/gamepadc.h"

using namespace love::common;

love::Type Gamepad::type("Joystick", &Object::type);

Gamepad::Gamepad(size_t id) : id(id), instanceID(-1), vibration()
{}

float Gamepad::ClampValue(float x)
{
    if (std::abs(x) < 0.01)
        return 0.0f;

    if (x < -0.99f)
        return -1.0f;
    if (x > 0.99f)
        return 1.0f;

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

bool Gamepad::GetConstant(const char* in, Gamepad::InputType& out)
{
    return inputTypes.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::InputType in, const char*& out)
{
    return inputTypes.Find(in, out);
}

// clang-format off
constexpr StringMap<Gamepad::InputType, Gamepad::INPUT_TYPE_MAX_ENUM>::Entry inputTypeEntries[] =
{
    { "axis",   Gamepad::INPUT_TYPE_AXIS   },
    { "button", Gamepad::INPUT_TYPE_BUTTON }
};

constinit const StringMap<Gamepad::InputType, Gamepad::INPUT_TYPE_MAX_ENUM> Gamepad::inputTypes(inputTypeEntries);
// clang-format on
